#ifndef AEEMOVIEFILE_H
#define AEEMOVIEFILE_H

/*============================================================================
FILE:   AEEMovieFile.h

SERVICES:  BREW Movie File Services

DESCRIPTION:
   This file defines the IMovieFile interface that is used to read and write
   audio and video stream data in a movie file.
   
PUBLIC CLASSES:  IMovieFile

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

    Copyright © 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

/*-------------------------------------------------------------------
            Include Files
-------------------------------------------------------------------*/

#include "AEE.h"

#ifdef FEATURE_MP4_TRANSCODER
#include "mp4_transcoder.h"
#endif
/*-------------------------------------------------------------------
            Macros
-------------------------------------------------------------------*/

// Reserved unique class ID for IMovieFile interface
#define AEECLSID_MOVIEFILE         0x010127d8

// These class IDs cover track types not otherwise covered in other parts of
// BREW.  The class IDs covered elsewhere are:
//     AEECLSID_MEDIA    - placeholder for "unknown" stream formats
//     AEECLSID_MEDIAQCP - QCP audio, with subtypes defined as follows:
//         MM_QCP_FORMAT_FIXED_FULL_13K  - IS-733 QCELP-13k voice full rate
//         MM_QCP_FORMAT_FIXED_HALF_13K  - IS-733 QCELP-13k voice half rate
//         MM_QCP_FORMAT_FIXED_FULL_EVRC - IS-127 EVRC voice
//         MM_QCP_FORMAT_FIXED_12_2_AMR  - GSM Advanced Multi-Rate
#define AEECLSID_MPEG4_VIDEO       0x010127fd  // MPEG-4 video (M4V)
#define AEECLSID_MPEG4_DATA        0x010127fe  // BIFS, OD, etc.
#define AEECLSID_MPEG4_AUDIO       0x010127ff  // Advanced Audio Codec (AAC)
#define AEECLSID_3GPP_TEXT         0x010268a7  // 3GPP Timed Text

// These are stream subtypes applicable to AEECLSID_MPEG4_DATA.
#define MOVIEFILE_M4S_TYPE_BIFS    0  // MPEG-4 scene descriptors
#define MOVIEFILE_M4S_TYPE_OD      1  // MPEG-4 object descriptors

// These are file brands
#define MOVIEFILE_BRAND_MP4 0
#define MOVIEFILE_BRAND_AMC 1
#define MOVIEFILE_BRAND_3GP 2
#define MOVIEFILE_BRAND_3G2 3
#define MOVIEFILE_BRAND_K3G 4
#define MOVIEFILE_BRAND_SKM 5

// These are the valid settings for the dwScrollType field of the structure
// AEEMovieTextSegment
#define MOVIEFILE_TEXT_SCROLL_VERT_UP     0
#define MOVIEFILE_TEXT_SCROLL_HORZ_LEFT   1
#define MOVIEFILE_TEXT_SCROLL_VERT_DOWN   2
#define MOVIEFILE_TEXT_SCROLL_HORZ_RIGHT  3

// These are the valid settings for the dwHorzJustify and dwVertJustify fields
// of the structure AEEMovieTextSegment
#define MOVIEFILE_TEXT_JUSTIFY_LEFT_OR_TOP      0
#define MOVIEFILE_TEXT_JUSTIFY_CENTER           1
#define MOVIEFILE_TEXT_JUSTIFY_RIGHT_OR_BOTTOM  2

#ifdef FEATURE_MP4_TRANSCODER
#define  IMOVIEFILE_TRANSCODER_EVENT_BASE          EVT_USER    // Base used by ICamera



#define IMOVIEFILE_TRANSCODER_STATE_INIT_DONE           (IMOVIEFILE_TRANSCODER_EVENT_BASE + 1)
#define IMOVIEFILE_TRANSCODER_STATE_AUDIO_START         (IMOVIEFILE_TRANSCODER_EVENT_BASE + 2) 
#define IMOVIEFILE_TRANSCODER_STATE_AUDIO_DONE          (IMOVIEFILE_TRANSCODER_EVENT_BASE + 3) 
#define IMOVIEFILE_TRANSCODER_STATE_VIDEO_CONT          (IMOVIEFILE_TRANSCODER_EVENT_BASE + 4)
#define IMOVIEFILE_TRANSCODER_STATE_DONE                (IMOVIEFILE_TRANSCODER_EVENT_BASE + 5)
#define IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_REQUEST (IMOVIEFILE_TRANSCODER_EVENT_BASE + 6)
#define IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_DONE    (IMOVIEFILE_TRANSCODER_EVENT_BASE + 7)
#define IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_RESET   (IMOVIEFILE_TRANSCODER_EVENT_BASE + 8)
#define IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_REQUEST (IMOVIEFILE_TRANSCODER_EVENT_BASE + 9)
#define IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_DONE    (IMOVIEFILE_TRANSCODER_EVENT_BASE + 10)
#define IMOVIEFILE_TRANSCODER_STATE_ERROR               (IMOVIEFILE_TRANSCODER_EVENT_BASE + 11)
#define IMOVIEFILE_TRANSCODER_STATE_TERMINATED          (IMOVIEFILE_TRANSCODER_EVENT_BASE + 12)
#endif

/*-------------------------------------------------------------------
            Structure Type Declarations
-------------------------------------------------------------------*/

typedef struct IMovieFile  IMovieFile;

// This structure contains information about an individual sample (frame) in a
// movie stream.
typedef struct AEEMovieSampleInfo
{
    uint32   cbSize;   // size of this structure, in bytes
    uint32   dwBytes;  // total bytes in sample
    uint32   dwTime;   // composition (display) time of sample, in time units
    uint32   dwDelta;  // number of time units between composition (display)
                       // times of this sample and the next sample
    uint32   dwSegment;  // sample description index
    flg      bSync:1;  // random access point, if true
} AEEMovieSampleInfo;

// This structure contains information about an individual stream in a movie
// file.
typedef struct AEEMovieStreamInfo
{
    uint32    cbSize;          // size of this structure, in bytes
    AEECLSID  format;          // stream format - see above for acceptable IDs
    uint32    nSubFormat;      // depends on stream format - see above
    uint16    nWidth;          // width in pixels (video/text only)
    uint16    nHeight;         // height in pixels (video/text only)
    uint32    nTimescale;      // time units per second
    uint32    dwRate;          // average frame rate in time units per frame
    uint32    dwLength;        // total stream length in time units
    uint32    dwFrames;        // total stream length in frames
    uint32    dwBytes;         // total bytes in stream
    uint32    dwLargestFrame;  // size of largest frame in bytes
    uint32    dwHeaderSize;    // size of header in bytes
    uint16    nOriginX;        // X-offset of output in display (video/text)
    uint16    nOriginY;        // Y-offset of output in display (video/text)
    char      rgchLanguage[3]; // ISO-639-2/T language code
} AEEMovieStreamInfo;

// This structure contains information about a movie file.
typedef struct AEEMovieFileInfo
{
    uint32              cbSize;     // size of this structure, in bytes
    AEECLSID            format;     // media format identifier
    uint16              nStreams;   // number of streams in file
	uint32              nMovieTimeScale;  //timescale of the movie
	uint32              nBrand;     //Brand of the file
    AEEMovieStreamInfo  *pStreams;  // pointer to array of stream structures
} AEEMovieFileInfo;

// This structure provides information about a font for the font table of a
// 3GPP timed text stream.
typedef struct AEEMovieTextFont
{
  int16                    wFontId;
  const char               *pszFontName;
} AEEMovieTextFont;

// This structure provides information necessary to construct a sample entry
// atom for a 3GPP timed text track.
typedef struct AEEMovieTextSegment
{
    flg                    fScrollIn:1;
    flg                    fScrollOut:1;
    flg                    fContinuousKaraoke:1;
    flg                    fWriteVertically:1;
    flg                    fBold:1;
    flg                    fItalic:1;
    flg                    fUnderline:1;
    uint32                 dwScrollType;
    uint32                 dwHorzJustify;
    uint32                 dwVertJustify;
    RGBVAL                 clrBack;
    int16                  wDefaultBoxLeft;
    int16                  wDefaultBoxTop;
    int16                  wDefaultBoxRight;
    int16                  wDefaultBoxBottom;
    uint16                 wFontId;
    uint8                  nFontSize;
    RGBVAL                 clrText;
    uint16                 cFonts;
    const AEEMovieTextFont *rgFonts;
} AEEMovieTextSegment;

// This structure contains information about a style record entry in a style
// text modifier for a 3GPP timed text stream sample.
typedef struct AEEMovieTextStyle
{
    uint16   iStart;
    uint16   iEnd;
    uint16   wFontId;
    flg      fBold:1;
    flg      fItalic:1;
    flg      fUnderline:1;
    uint8    nFontSize;
    RGBVAL   clrText;
} AEEMovieTextStyle;

// This structure contains information about an entry in a dynamic highlight
// (karaoke) text modifier for a 3GPP timed text stream sample.
typedef struct AEEMovieTextKaraoke
{
    uint32  dwEndTime;
    uint16  iStart;
    uint16  iEnd;
} AEEMovieTextKaraoke;

//===========================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===========================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

//***************************************************************************
//
// IMovieFile Interface
//
//***************************************************************************
AEEINTERFACE(IMovieFile)
{
   INHERIT_IQueryInterface(IMovieFile);

   // This method is used to open an existing movie for reading, or create a
   // new movie for writing.
   int     (*Open) (IMovieFile * po, const char * pszFileName,
                    boolean bRecord, AEECLSID format,
                    AEEMovieFileInfo * pInfo);

   // These methods are used to read data from a movie.
   int     (*ReadUserAtom) (IMovieFile * po, uint16 nAtom,
                            void * pBuffer, uint32 * pnSize);
   int     (*ReadStream) (IMovieFile * po, uint16 nStream,
                          uint32 nSamples, uint32 nOffset,
                          void * pBuffer, uint32 * pnSize);
   int     (*GetSampleInfo) (IMovieFile * po, uint16 nStream,
                             uint32 nSamples, uint32 nOffset,
                             AEEMovieSampleInfo * pInfo);

   // These methods are used to write data to a movie.
   int     (*WriteUserAtom) (IMovieFile * po, const void * pBuffer,
                             uint32 dwSize);
   int     (*WriteStream) (IMovieFile * po, uint16 nStream,
                           uint32 nSamples,
                           const AEEMovieSampleInfo * pSampleInfo,
                           const void * pBuffer);

#ifdef FEATURE_MP4_TRANSCODER
   int     (*TranscoderStart) (IMovieFile * po, //output file
                              IMovieFile * pi, //output file
                              const char *pszInputFile,
                              const char *pszOutputFile,
                              xcoder_audio_type        target_audio_format, 
                              xcoder_video_type        target_video_format, 
                              AEEMovieFileInfo *temp);
#endif
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
   byte* (*SetVOPTime)(IMovieFile *po, int32 delta, byte *buffer, uint32 *sizes);
   int   (*GetVOPTime)(IMovieFile *po, byte *buffer, uint32 size);
   int   (*GetTimeIncRes)(IMovieFile *po, byte *header, int size);

#endif

   // This method is used to start a new 3GPP timed text stream segment, as
   // described by a new 3GPP Timed Text Sample Entry (tx3g) atom in the movie
   // meta data.
   int     (*TextNewSegment) (IMovieFile * po, uint16 nStream,
                              const AEEMovieTextSegment * pSegment);

   // These methods are used to create, destroy, and submit samples for a 3GPP
   // timed text stream.
   int     (*TextNewSample) (IMovieFile * po, const char * rgchText,
                             uint16 cchText, void ** phSample);
   int     (*TextDeleteSample) (IMovieFile * po, void * hSample);
   int     (*TextSubmitSample) (IMovieFile * po, void * hSample,
                                uint16 nStream, uint32 dwDelta);

   // These methods are used to apply modifiers to 3GPP timed text stream
   // samples before submitting them.
   int     (*TextModifyStyle) (IMovieFile * po, void * hSample,
                               const AEEMovieTextStyle * rgStyle,
                               uint16 cStyle);
   int     (*TextModifyHighlight) (IMovieFile * po, void * hSample,
                                   uint16 iStart, uint16 iEnd);
   int     (*TextModifyHighlightColor) (IMovieFile * po, void * hSample,
                                        RGBVAL color);
   int     (*TextModifyKaraoke) (IMovieFile * po, void * hSample,
                                 uint32 dwStart,
                                 const AEEMovieTextKaraoke * rgKaraoke,
                                 uint16 cKaraoke);
   int     (*TextModifyScroll) (IMovieFile * po, void * hSample,
                                uint32 dwScrollDelay);
   int     (*TextModifyLink) (IMovieFile * po, void * hSample,
                              uint16 iStart, uint16 iEnd,
                              const char * rgchURL, uint8 cchURL,
                              const char * rgchAlt, uint8 cchAlt);
   int     (*TextModifyBox) (IMovieFile * po, void * hSample,
                             int16 nLeft, int16 nTop,
                             int16 nRight, int16 nBottom);
   int     (*TextModifyBlink) (IMovieFile * po, void * hSample,
                               uint16 iStart, uint16 iEnd);
   int     (*TextModifyWrap) (IMovieFile * po, void * hSample,
                              flg fAutomatic);

   // Methods for reading/writing raw stream segment headers directly (for
   // timed text streams)
   int     (*ReadSegmentHeader) (IMovieFile * po, uint16 nStream,
                                 uint16 nSegment,
                                 void * pBuffer, uint32 * pnSize);
   int     (*WriteSegmentHeader) (IMovieFile * po, uint16 nStream,
                                  const void * pBuffer, uint32 dwSize);
};

#define IMOVIEFILE_AddRef(p) \
    AEEGETPVTBL(p, IMovieFile)->AddRef(p)
#define IMOVIEFILE_Release(p) \
    AEEGETPVTBL(p, IMovieFile)->Release(p)
#define IMOVIEFILE_QueryInterface(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->QueryInterface(p,a,b)
#define IMOVIEFILE_Open(p,a,b,c,d) \
    AEEGETPVTBL(p, IMovieFile)->Open(p,a,b,c,d)
#define IMOVIEFILE_OpenMp4Read(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->Open(p,a,FALSE,AEECLSID_MEDIAMPEG4,b)
#define IMOVIEFILE_OpenMp4Write(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->Open(p,a,TRUE,AEECLSID_MEDIAMPEG4,b)
#define IMOVIEFILE_ReadUserAtom(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->ReadUserAtom(p,a,b,c)
#define IMOVIEFILE_ReadStream(p,a,b,c,d,e) \
    AEEGETPVTBL(p, IMovieFile)->ReadStream(p,a,b,c,d,e)
#define IMOVIEFILE_ReadStreamSample(p,a,b,c,d) \
    AEEGETPVTBL(p, IMovieFile)->ReadStream(p,a,1,b,c,d)
#define IMOVIEFILE_GetSampleInfo(p,a,b,c,d) \
    AEEGETPVTBL(p, IMovieFile)->GetSampleInfo(p,a,b,c,d)
#define IMOVIEFILE_GetOneSampleInfo(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->GetSampleInfo(p,a,1,b,c)
#define IMOVIEFILE_WriteUserAtom(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->WriteUserAtom(p,a,b)
#define IMOVIEFILE_WriteStream(p,a,b,c,d) \
    AEEGETPVTBL(p, IMovieFile)->WriteStream(p,a,b,c,d)
#ifdef FEATURE_MP4_TRANSCODER
#define IMOVIEFILE_TranscoderStart(p,a,b,c,d,e,f) \
    AEEGETPVTBL(p, IMovieFile)->TranscoderStart(p,a,b,c,d,e,f)    
#endif
#define IMOVIEFILE_WriteStreamSample(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->WriteStream(p,a,1,b,c)
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
#define IMOVIEFILE_SetVOPTime(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->SetVOPTime(p,a,b,c)
#define IMOVIEFILE_GetVOPTime(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->GetVOPTime(p,a,b)
#define IMOVIEFILE_GetTimeIncRes(p,a,b) \
	    AEEGETPVTBL(p, IMovieFile)->GetTimeIncRes(p,a,b)

#endif
#define IMOVIEFILE_TextNewSegment(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->TextNewSegment(p,a,b)
#define IMOVIEFILE_TextNewSample(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->TextNewSample(p,a,b,c)
#define IMOVIEFILE_TextDeleteSample(p,a) \
    AEEGETPVTBL(p, IMovieFile)->TextDeleteSample(p,a)
#define IMOVIEFILE_TextSubmitSample(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->TextSubmitSample(p,a,b,c)
#define IMOVIEFILE_TextModifyStyle(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyStyle(p,a,b,c)
#define IMOVIEFILE_TextModifyHighlight(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyHighlight(p,a,b,c)
#define IMOVIEFILE_TextModifyHighlightColor(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyHighlightColor(p,a,b)
#define IMOVIEFILE_TextModifyKaraoke(p,a,b,c,d) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyKaraoke(p,a,b,c,d)
#define IMOVIEFILE_TextModifyScroll(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyScroll(p,a,b)
#define IMOVIEFILE_TextModifyLink(p,a,b,c,d,e,f,g) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyLink(p,a,b,c,d,e,f,g)
#define IMOVIEFILE_TextModifyBox(p,a,b,c,d,e) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyBox(p,a,b,c,d,e)
#define IMOVIEFILE_TextModifyBlink(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyBlink(p,a,b,c)
#define IMOVIEFILE_TextModifyWrap(p,a,b) \
    AEEGETPVTBL(p, IMovieFile)->TextModifyWrap(p,a,b)
#define IMOVIEFILE_ReadSegmentHeader(p,a,b,c,d) \
    AEEGETPVTBL(p, IMovieFile)->ReadSegmentHeader(p,a,b,c,d)
#define IMOVIEFILE_WriteSegmentHeader(p,a,b,c) \
    AEEGETPVTBL(p, IMovieFile)->WriteSegmentHeader(p,a,b,c)


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 
AEEMovieSampleInfo

  Description:

This structure contains information about an individual sample (frame) in
a movie stream.  It is obtained by using the IMOVIEFILE_GetSampleInfo()
method on an open movie stream.

  Definition:

typedef struct AEEMovieSampleInfo
{
    uint32   cbSize;   // size of this structure, in bytes
    uint32   dwBytes;  // total bytes in sample
    uint32   dwTime;   // composition (display) time of sample, in time units
    uint32   dwDelta;  // number of time units between composition (display)
                       // times of this sample and the next sample
    uint32   dwSegment;  // sample description index
    flg    bSync:1;    // random access point, if true
} AEEMovieSampleInfo;

  Members:

cbSize:     This specifies the total size of this structure, in bytes.
dwBytes:    This is the total number of bytes in the encoded sample.
dwTime:     This is the composition or display time of the sample, in time
            units.  To convert time units into seconds, divide by the time
            scale of the stream, available in the AEEMovieStreamInfo
            structure for the stream.
dwDelta:    This is the difference between composition or display time of
            this sample and the next sample in the stream, in time units.
            To convert time units into seconds, divide by the time
            scale of the stream, available in the AEEMovieStreamInfo
            structure for the stream.
dwSegment:  This indicates which segment of the stream the sample belongs to.
            A segment is defined by an associated header, also called a
            "sample entry", which can be read using
            IMOVIEFILE_ReadSegmentHeader().  Normally, video/audio streams
            have only one segment/header.  However, certain types of streams,
            such as timed text, can have a new segment/header every time
            default rendering settings change mid-stream.
bSync:      If "true", this indicates the sample is a random access point, or
            a sample in the stream which can be decoded without first
            decoding any samples before it in the stream.  This type of sample
            sometimes referred to as an "I-Frame", as opposed to a "P-Frame"
            or "B-Frame", which are predicted from other samples and so cannot
            be decoded without first decoding the other samples by which the
            sample is predicted.

  Comments:

A "sample" is the smallest unit of data into which a stream may be divided
without fragmenting the data input to the stream's decoder.  This is also
known as a "frame".  Typically a single sample or frame decodes into
multiple units of decoded audio or video information.  For audio, an encoded
sample or frame is sometimes called a "packet".

For some types of stream, the samples all have the same size in bytes.
For others, each sample may be a different size.  The size of the largest
sample in the stream may be determined by looking at the "dwLargestFrame"
member of the AEEMovieStreamInfo structure for the stream.

  See Also:

AEEMovieStreamInfo, IMOVIEFILE_GetSampleInfo().

=============================================================================
AEEMovieStreamInfo

  Description:

This structure contains information about an individual stream in a movie
file.  It is returned to the user, along with the structures for the rest
of the streams in the movie, by using the IMOVIEFILE_Open() method to open an
existing movie file for reading.  When writing a movie, this structure must
be filled in by the user before calling IMOVIEFILE_Open().

  Definition:

typedef struct AEEMovieStreamInfo
{
    uint32    cbSize;          // size of this structure, in bytes
    AEECLSID  format;          // stream format - see below for acceptable IDs
    uint32    nSubFormat;      // depends on stream format - see below
    uint16    nWidth;          // width in pixels (video only)
    uint16    nHeight;         // height in pixels (video only)
    uint32    nTimescale;      // time units per second
    uint32    dwRate;          // average frame rate in time units per frame
    uint32    dwLength;        // total stream length in time units
    uint32    dwFrames;        // total stream length in frames
    uint32    dwBytes;         // total bytes in stream
    uint32    dwLargestFrame;  // size of largest frame in bytes
    uint32    dwHeaderSize;    // size of header in bytes
    uint16    nOriginX;        // X-offset of output in display (video/text)
    uint16    nOriginY;        // Y-offset of output in display (video/text)
    char      rgchLanguage[3]; // ISO-639-2/T language code
} AEEMovieStreamInfo;

  Members:

cbSize:          This specifies the total size of this structure, in bytes.
format:          This identifies the format of the stream.  The currently
                 accepted IDs are:
             AEECLSID_MEDIA       - placeholder for "unknown" stream formats
             AEECLSID_MEDIAQCP    - QCP audio
             AEECLSID_MPEG4_VIDEO - MPEG-4 video (M4V)
             AEECLSID_MPEG4_DATA  - BIFS, OD, etc.
             AEECLSID_MPEG4_AUDIO - Advanced Audio Codec (AAC)
nSubFormat:      For those stream formats with more than one possible
                 configuration, this specifies the exact format of the stream,
                 as follows:
         AEECLSID_MEDIAQCP:
             MM_QCP_FORMAT_FIXED_FULL_13K  - IS-733 QCELP-13k voice full rate
             MM_QCP_FORMAT_FIXED_HALF_13K  - IS-733 QCELP-13k voice half rate
             MM_QCP_FORMAT_FIXED_FULL_EVRC - IS-127 EVRC voice
             MOVIEFILE_QCP_TYPE_AMR        - GSM Advanced Multi-Rate
         AEECLSID_MPEG4_DATA:
             MOVIEFILE_M4S_TYPE_BIFS       - MPEG-4 scene descriptors
             MOVIEFILE_M4S_TYPE_OD         - MPEG-4 object descriptors
nWidth:          For video streams, this is the width of the output video
                 data for one sample or frame, in units of pixels.
nHeight:         For video streams, this is the height of the output video
                 data for one sample or frame, in units of pixels.
nTimescale:      This is the number of time units per second for this stream.
                 This is used as the divisor for all other time quantities
                 associated with the stream, and is useful for representing
                 fractional numbers of seconds.
dwRate:          This is the average time between samples in the stream,
                 in time units.  For fixed-rate streams, this is the fixed
                 time interval between samples.  Otherwise, this is an overall
                 average obtained by dividing the length by the n
dwLength:        This is the length of the stream, in time units.
dwFrames:        This is the length of the stream, in samples (frames).
dwBytes:         This is the total number of bytes in the stream.
dwLargestFrame:  This is the size of the largest sample in the stream,
                 in bytes.
dwHeaderSize:    This is the size of the stream header, in bytes.
nOriginX:        For video/text streams, this is the X-offset where the
                 output video data should be drawn on the screen, in pixels.
nOriginY:        For video/text streams, this is the Y-offset where the
                 output video data should be drawn on the screen, in pixels.
rgchLanguage:    Language code to set in header for the track.

  Comments:

The stream header is also known as the "decoder specific information".  It is
the data typically given to the decoder for the stream, during initialization
time, in order for the decoder to be properly configured to decode the stream.

Information about individual samples in a stream are available in the
AEEMovieSampleInfo structure for a sample.  This may be obtained by using
the IMOVIEFILE_GetSampleInfo() method.

  See Also:

AEEMovieStreamFormat, AEEMovieSampleInfo, IMOVIEFILE_Open(),
IMOVIEFILE_ReadStream().

=============================================================================
AEEMovieFileInfo

  Description:

This structure contains information about a movie file.  It is returned to
the user when an existing movie file is opened with the IMOVIEFILE_Open()
method, and must be filled in by the user if calling the IMOVIEFILE_Open()
method to create a new movie.

  Definition:

typedef struct AEEMovieFileInfo
{
    uint32              cbSize;           // size of this structure, in bytes
    AEECLSID            format;           // media format identifier
    uint16              nStreams;         // number of streams in file
    AEEMovieStreamInfo  streams [AEE_MAX_MOVIE_STREAMS];
} AEEMovieFileInfo;

  Members:

cbSize:          This specifies the total size of this structure, in bytes.
format:          This identifies the format of the stream.  Currently the
                 only supported movie format is AEECLSID_MEDIAMPEG4, which
                 represents an MP4 file (MPEG-4 movie file).  The AMC
                 (movie mail) and 3GP (3GPP2) file formats have the same
                 structure as MP4 and thus are also supported.
nStreams:        This is the total number of streams in the movie file.
streams:         This is an array of stream information structures.  These
                 structures contain the information about each individual
                 stream in the movie.

  Comments:

The movie format identifier AEECLSID_MEDIAMPEG4 is defined in the
AEEClassIds.h file.

  See Also:

AEEMovieStreamInfo, IMOVIEFILE_Open().

=============================================================================

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

Interface Name: IMovieFile

Description: 
    IMovieFile interface provides a generic way to BREW applications to 
    read and write movie files in various formats like MPEG4.  A movie file
    is a media file containing separate streams of audio and/or video, which
    may be played separately or together.

    This interface may be used in two different modes, determined at the time
    the file is opened using IMOVIEFILE_Open().

    In "read" mode, the interface is used to open an already existing movie
    file for reading only.  The IMOVIEFILE_WriteUserAtom() and
    IMOVIEFILE_WriteStream() methods are unavailable in this mode.  In this
    mode, information about samples can be retrieved using
    IMOVIEFILE_GetSampleInfo(), sample data may be read using
    IMOVIEFILE_ReadStream(), and user atom data may be read using
    IMOVIEFILE_ReadUserAtom().  There is no required order of operations,
    and these three methods may be called in any order or frequency.
    Furthermore, data may be read in any order, or randomly accessed.

    In "write" mode, the interface is used to create a new movie file for
    writing only.  The IMOVIEFILE_GetSampleInfo(), IMOVIEFILE_ReadUserAtom(),
    and IMOVIEFILE_ReadStream() methods are unavailable in this mode.  In
    this mode, user atoms may be written first by calling
    IMOVIEFILE_WriteUserAtom() one or more times.  Next, stream data may be
    written by calling IMOVIEFILE_WriteStream() repeatedly.  The data
    written to streams must be sequential - no random access writing
    is allowed.

=============================================================================

Function:  IMOVIEFILE_AddRef()

Description: 
   This function increments the reference count of the ICamera Interface
   object.  This allows the object to be shared by multiple callers.  The
   object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IMOVIEFILE_AddRef (IMovieFile * po)

Parameters:
 
   po : Pointer to the IMovieFile interface object

Return Value:
   Incremented reference count for the object

Comments: 
   A valid object returns a positive reference count.

Side Effects: 
   None

See Also:
   IMOVIEFILE_Release() 

=============================================================================

Function:  IMOVIEFILE_Release()

Description: 
   This function decrements the reference count of an object.  The object is
   freed from memory and is no longer valid once the reference count
   reaches 0 (zero).

Prototype:
   uint32 IMOVIEFILE_Release (IMovieFile * po)

Parameters:
 
   po: Pointer to the ICamera Interface object

Return Value: 
   Decremented reference count for the object. The object has been freed and
   is no longer valid if 0 (zero) is returned. 

Comments: 
   None

Side Effects: 
   None

See Also: 
   IMOVIEFILE_AddRef() 

=============================================================================

Function:  IMOVIEFILE_QueryInterface()

Description:
   This function can be used to 
   (1) get a pointer to an interface or data based on the input class ID
   (2) query an extended version of the IMovieFile-derived class
   (3) support version compatibility

Prototype:
   int IMOVIEFILE_QueryInterface (IMovieFile * po, AEECLSID clsReq,
                                  void ** ppo)

Parameters:
   po [in]: Pointer to ICamera interface.

   clsReq [in]: A globally unique id to identify the entity (interface or
                data) that we are trying to query.

   ppo[out]: Pointer to the interface or data that we want to retrieve.
             If the value passed back is NULL, the interface or data that
             we query are not available.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments:
   If ppo is back a NULL pointer, the interface or data that we query is not
   available.

Side Effects: 
   If an interface is retrieved, then this function increments its reference
   count.  If a data structure is retrieved, then a pointer to the internal
   structure is given and user should not free it.

See Also: 
   None.

=============================================================================

Function:  IMOVIEFILE_Open()

Description:
   This function either opens an existing file for reading, or creates a
   new movie file for writing.

Prototype:
   int IMOVIEFILE_Open (IMovieFile * po, const char * pszFileName,
                        boolean bRecord, AEECLSID format,
                        AEEMovieFileInfo * pInfo)

Parameters:
   po [in]:           Pointer to the IMovieFile interface object
   pszFileName [in]:  Pointer to the name of the movie file to open
   bRecord [in]:      Indicates whether the movie file is an existing movie
                      file to be opened for reading (FALSE) or a new movie
                      file to be created for writing (TRUE)
   format [in]:       Indicates the format of the movie file.  Currently the
                      only supported format is AEECLSID_MEDIAMPEG4, which
                      indicates an MPEG-4 movie format file.  The AMC
                      (movie mail) and 3G2 (3GPP2) file formats use the same
                      structure and thus are also supported.
   pInfo [in/out]:    This is an output parameter if bRecord is set to "FALSE",
                      indicating an existing movie file is being opened for
                      reading.  In this case, information about the existing
                      movie is stored in this structure.  If bRecord is set
                      to "TRUE", indicating a new movie file is being
                      created for writing, this parameter is an input
                      parameter, and the user must fill in the structure
                      indicating what is about to be written to the movie.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   This method has two uses - for opening an existing movie file for reading,
   or creating a new movie file for writing.  Opening an existing movie file
   for writing is not supported.  The user selects either read or write mode
   through the bRecord parameter.

   If creating a new movie file for writing, not all the fields in the
   individual stream structures under the pInfo structure need to be
   filled out.  In general, only cbSize, format, nSubFormat, dwHeaderSize,
   and nTimescale need to be filled out.  For video streams, nWidth and
   nHeight must also be filled out.

Side Effects: 
   None.

See Also:
   AEEMovieFileInfo.

=============================================================================

Function:  IMOVIEFILE_OpenMp4Read()

Description:
   This function is a convenient wrapper for the IMOVIEFILE_Open() function.
   It opens an existing MP4 file for reading.

Prototype:
   int IMOVIEFILE_OpenMp4Read (IMovieFile * po, const char * pszFileName,
                               AEEMovieFileInfo * pInfo)

Parameters:
   po [in]:           Pointer to the IMovieFile interface object
   pszFileName [in]:  Pointer to the name of the MP4 file to open for reading
   pInfo [out]:       Information about the existing movie is stored in
                      this structure.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   None.

Side Effects: 
   None.

See Also:
   AEEMovieFileInfo, IMOVIEFILE_Open().

=============================================================================

Function:  IMOVIEFILE_OpenMp4Write()

Description:
   This function is a convenient wrapper for the IMOVIEFILE_Open() function.
   It creates a new MP4 file for writing.

Prototype:
   int IMOVIEFILE_OpenMp4Write (IMovieFile * po, const char * pszFileName,
                                const AEEMovieFileInfo * pInfo)

Parameters:
   po [in]:           Pointer to the IMovieFile interface object
   pszFileName [in]:  Pointer to the name of the MP4 file to create
   pInfo [in]:        This points to a structure indicating what is about
                      to be written to the movie.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   The pInfo structure needs to be carefully filled in such a way that the
   sampling rate for certain streams can be correctly determined.
   For instance, in video streams, the nTimescale, dwLength, and dwFrames
   fields need to be filled out.  If the actual stream length is not known,
   use the value of "1" for dwFrames, and for dwLength use the frame
   period, in units of the timescale.  The dwBytes and dwLargestFrame are
   not required to be filled in.

Side Effects: 
   None.

See Also:
   AEEMovieFileInfo, IMOVIEFILE_Open().

=============================================================================

Function:  IMOVIEFILE_ReadUserAtom()

Description:
   This function is used to read one of the user atoms in the movie file
   meta-data.  These atoms become available for reading as soon as the file
   is opened for reading using IMOVIEFILE_Open(), and remain available even
   after the streams in the file are read using IMOVIEFILE_ReadStream().

   The movie file must be an existing movie file opened for reading.
   See IMOVIEFILE_Open() for more details.

   A user atom is used to contain movie header information such as GPS data
   or copyright notices for the movie.  Specific user atoms are defined by
   3GPP2 for use in movie files.  These atoms are not parsed or generated
   by this interface, but must be handled by the user.

Prototype:
   int IMOVIEFILE_ReadUserAtom (IMovieFile * po, uint16 nAtom,
                                void * pBuffer, uint32 * pnSize)

Parameters:
   po [in]:                Pointer to the IMovieFile interface object
   uint16 nAtom [in]:      Sequence number of the user atom in the file.
                           The first user atom in the file has a sequence
                           number of zero, the next user atom after that has a
                           sequence number of one, and so on.
   void * pBuffer [out]:   This is a pointer to where the user atom data
                           should be stored.  If NULL, no data is actually
                           stored.
   uint32 * pnSize [out]:  This is where the total size of the atom in bytes
                           is stored.  If this is set to zero, then there is
                           no user atom with the given sequence number.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   The user must be sure to allocate enough space in the buffer for storing
   the data from the user atom.  To determine the amount of space required,
   the user can call the function with NULL for the buffer pointer, in which
   case the data is not stored but the total size of the atom is still
   set in the variable pointed to by pnSize.

   The user can determine the number of user atoms in the file by reading
   with sequence numbers starting at zero and increasing until the returned
   atom size is zero, indicating there are no more user atoms in the file.

   In the data buffer for the user atom, the first 16 bytes contain the UUID
   of the user atom.  This uniquely identifies the type of the user atom.

Side Effects: 
   None.

See Also:
   IMOVIEFILE_Open(), IMOVIEFILE_ReadStream().

=============================================================================

Function:  IMOVIEFILE_ReadStream()

Description:
   This function reads one or more samples (frames) from one of the streams
   in the movie file.

   The movie file must be an existing movie file opened for reading.
   See IMOVIEFILE_Open() for more details.

Prototype:
   int IMOVIEFILE_ReadStream (IMovieFile * po, uint16 nStream,
                              uint32 nSamples, uint32 nOffset,
                              void * pBuffer, uint32 * pnSize)

Parameters:
   po [in]:                Pointer to the IMovieFile interface object
   uint16 nStream [in]:    This is the index of the stream to read.  The index
                           is used to look up the stream in the array in
                           the AEEMovieFileInfo structure given to the user
                           when IMOVIEFILE_Open() is called.
   uint32 nSamples [in]:   This is the number of samples or frames to read
                           from the file.
   uint32 nOffset [in]:    This is the sample or frame number at which to
                           begin reading the stream.  The first sample in the
                           file has an offset of zero.
   void * pBuffer [out]:   This is a pointer to the buffer in which to store
                           the data from the stream.  The user must be sure
                           to allocate enough space for this buffer.  If NULL
                           is passed in, no data is actually stored.
   uint32 * pnSize [out]:  If the given buffer pointer is NULL, the number
                           of bytes required for the buffer is stored here.
                           Otherwise, the number of bytes read is stored here.

Return Value:
   Return SUCCESS on success, otherwise returns error code.
   
Comments: 
   The user must be sure to allocate enough space in the buffer for storing
   the data from the stream.  To determine the amount of space required,
   the user can call the function with NULL for the buffer pointer, in which
   case the data is not stored but the total size of the data from the given
   number of samples is still set in the variable pointed to by pnSize.

Side Effects: 
   None.

See Also:
   AEEMovieFileInfo, IMOVIEFILE_Open().

=============================================================================

Function:  IMOVIEFILE_ReadStreamSample()

Description:
   This function is a convenient wrapper for the IMOVIEFILE_ReadStream()
   function.  It reads one sample (frame) from one of the streams
   in the movie file.

   The movie file must be an existing movie file opened for reading.
   See IMOVIEFILE_Open() for more details.

Prototype:
   int IMOVIEFILE_ReadStreamSample (IMovieFile * po, uint16 nStream,
                                    uint32 nOffset,
                                    void * pBuffer, uint32 * pnSize)

Parameters:
   po [in]:                Pointer to the IMovieFile interface object
   uint16 nStream [in]:    This is the index of the stream to read.  The index
                           is used to look up the stream in the array in
                           the AEEMovieFileInfo structure given to the user
                           when IMOVIEFILE_Open() is called.
   uint32 nOffset [in]:    This is the sample or frame number at which to
                           begin reading the stream.  The first sample in the
                           file has an offset of zero.
   void * pBuffer [out]:   This is a pointer to the buffer in which to store
                           the data from the stream.  The user must be sure
                           to allocate enough space for this buffer.  If NULL
                           is passed in, no data is actually stored.
   uint32 * pnSize [out]:  If the given buffer pointer is NULL, the number
                           of bytes required for the buffer is stored here.
                           Otherwise, the number of bytes read is stored here.

Return Value:
   Return SUCCESS on success, otherwise returns error code.
   
Comments: 
   The user must be sure to allocate enough space in the buffer for storing
   the data from the stream.  To determine the amount of space required,
   the user can call the function with NULL for the buffer pointer, in which
   case the data is not stored but the total size of the data from the given
   number of samples is still set in the variable pointed to by pnSize.

Side Effects: 
   None.

See Also:
   AEEMovieFileInfo, IMOVIEFILE_Open(), IMOVIEFILE_ReadStream().

=============================================================================

Function:  IMOVIEFILE_GetSampleInfo()

Description:
   This function returns information about one or more samples in a stream
   of the movie file.  See AEEMovieSampleInfo for more information.

   The movie file must be an existing movie file opened for reading.
   See IMOVIEFILE_Open() for more details.

Prototype:
   int IMOVIEFILE_GetSampleInfo (IMovieFile * po, uint16 nStream,
                                 uint32 nSamples, uint32 nOffset,
                                 AEEMovieSampleInfo * pInfo)

Parameters:
   po [in]:                           Pointer to the IMovieFile interface
                                      object
   uint16 nStream [in]:               This is the index of the stream to
                                      read.  The index is used to look up
                                      the stream in the array in the
                                      AEEMovieFileInfo structure given to
                                      the user when IMOVIEFILE_Open()
                                      is called.
   uint32 nSamples [in]:              This is the number of samples or
                                      frames for which to obtain information.
   uint32 nOffset [in]:               This is the index of the first sample
                                      or frame for which to obtain
                                      information.  The first sample in the
                                      file has an offset of zero.
   AEEMovieSampleInfo * pInfo [out]:  This is where the information about the
                                      samples is stored.  The user must be
                                      sure to allocate enough space for
                                      this buffer.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   None.

Side Effects: 
   None.

See Also:
   AEEMovieFileInfo, AEEMovieSampleInfo, IMOVIEFILE_Open().

=============================================================================

Function:  IMOVIEFILE_GetOneSampleInfo()

Description:
   This function is a convenient wrapper for the IMOVIEFILE_GetSampleInfo()
   function.  It returns information about one sample in a stream
   of the movie file.  See AEEMovieSampleInfo for more information.

   The movie file must be an existing movie file opened for reading.
   See IMOVIEFILE_Open() for more details.

Prototype:
   int IMOVIEFILE_GetOneSampleInfo (IMovieFile * po, uint16 nStream,
                                    uint32 nOffset,
                                    AEEMovieSampleInfo * pInfo)

Parameters:
   po [in]:                           Pointer to the IMovieFile interface
                                      object
   uint16 nStream [in]:               This is the index of the stream to
                                      read.  The index is used to look up
                                      the stream in the array in the
                                      AEEMovieFileInfo structure given to
                                      the user when IMOVIEFILE_Open()
                                      is called.
   uint32 nOffset [in]:               This is the index of the first sample
                                      or frame for which to obtain
                                      information.  The first sample in the
                                      file has an offset of zero.
   AEEMovieSampleInfo * pInfo [out]:  This is where the information about the
                                      samples is stored.  The user must be
                                      sure to allocate enough space for
                                      this buffer.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   None.

Side Effects: 
   None.

See Also:
   AEEMovieFileInfo, AEEMovieSampleInfo, IMOVIEFILE_Open(),
   IMOVIEFILE_GetSampleInfo().

=============================================================================

Function:  IMOVIEFILE_WriteUserAtom()

Description:
   This function is used to write one of the user atoms in the movie file
   meta-data.  These atoms can be written anytime after calling
   IMOVIEFILE_Open() but before calling IMOVIEFILE_WriteStream().

   The movie file must be a new movie file created for writing.
   See IMOVIEFILE_Open() for more details.

   A user atom is used to contain movie header information such as GPS data
   or copyright notices for the movie.  Specific user atoms are defined by
   3GPP2 for use in movie files.  These atoms are not parsed or generated
   by this interface, but must be handled by the user.

Prototype:
   int IMOVIEFILE_WriteUserAtom (IMovieFile * po, const void * pBuffer,
                                 uint32 dwSize)

Parameters:
   po [in]:                    Pointer to the IMovieFile interface object
   const void * pBuffer [in]:  This points to the data to be written out
                               to the user atom.
   uint32 dwSize [in]:         This is the size of the user atom to write out.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   In the data buffer for the user atom, the first 16 bytes contain the UUID
   of the user atom.  This uniquely identifies the type of the user atom.

   Atoms written using this method are written sequentially to the movie file.
   Writing user atoms in a random access fashion is not supported.

Side Effects: 
   None.

See Also:
   IMOVIEFILE_Open(), IMOVIEFILE_WriteStream().

=============================================================================

Function:  IMOVIEFILE_WriteStream()

Description:
   This function writes one or more samples (frames) to one of the streams
   in the movie file.

   The movie file must be a new movie file created for writing.
   See IMOVIEFILE_Open() for more details.

Prototype:
   int IMOVIEFILE_WriteStream (IMovieFile * po, uint16 nStream,
                               uint32 nSamples,
                               const AEEMovieSampleInfo * pSampleInfo,
                               const void * pBuffer);

Parameters:
   po [in]:                            Pointer to the IMovieFile interface
                                       object
   uint16 nStream [in]:                This is the index of the stream to
                                       write.  The index is used to look up
                                       the stream in the array in the
                                       AEEMovieFileInfo structure given to
                                       the user when IMOVIEFILE_Open()
                                       is called.
   uint32 nSamples [in]:               This is the number of samples or
                                       frames to write to the file.
   const uint32 * pnSampleSizes [in]:  This is a pointer to an array containing
                                       the sizes of the samples to be written
                                       to the file.  The array must be at least
                                       nSamples in size.
   const void * pBuffer [in]:          This is a pointer to the data to be
                                       written out to the stream.  The buffer
                                       must hold at least as many bytes as
                                       the total of the sample sizes indicated
                                       by the pnSampleSizes array.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   Once this method is called, the IMOVIEFILE_WriteUserAtom function will
   become unavailable.  This is because in the movie file layout the user
   atom data must be written first before any stream data.

   Because data from all the streams is interleaved when written to the
   movie file by the underlying stream writer, the user should write about
   the same time intervals of data to each stream evenly.  Writing too much
   data to one stream and not enough to another stream may result in the
   underlying movie writer's buffer for that stream overflowing,
   causing an error.

Side Effects: 
   As a result of this call, data may or may not be written to the movie file.
   This is because there is an internal buffer used to combine writes together
   and to perform stream interleaving.

See Also:
   AEEMovieFileInfo, IMOVIEFILE_Open(), IMOVIEFILE_WriteUserAtom().

=============================================================================

Function:  IMOVIEFILE_WriteStreamSample()

Description:
   This function is a convenient wrapper for the IMOVIEFILE_WriteStream()
   function.  It writes one sample (frame) to one of the streams
   in the movie file.

   The movie file must be a new movie file created for writing.
   See IMOVIEFILE_Open() for more details.

Prototype:
   int IMOVIEFILE_WriteStreamSample (IMovieFile * po, uint16 nStream,
                                     const AEEMovieSampleInfo * pSampleInfo,
                                     const void * pBuffer);

Parameters:
   po [in]:                            Pointer to the IMovieFile interface
                                       object
   uint16 nStream [in]:                This is the index of the stream to
                                       write.  The index is used to look up
                                       the stream in the array in the
                                       AEEMovieFileInfo structure given to
                                       the user when IMOVIEFILE_Open()
                                       is called.
   const uint32 * pnSampleSizes [in]:  This is a pointer to an array containing
                                       the sizes of the samples to be written
                                       to the file.  The array must be at least
                                       nSamples in size.
   const void * pBuffer [in]:          This is a pointer to the data to be
                                       written out to the stream.  The buffer
                                       must hold at least as many bytes as
                                       the total of the sample sizes indicated
                                       by the pnSampleSizes array.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   Once this method is called, the IMOVIEFILE_WriteUserAtom function will
   become unavailable.  This is because in the movie file layout the user
   atom data must be written first before any stream data.

   Because data from all the streams is interleaved when written to the
   movie file by the underlying stream writer, the user should write about
   the same time intervals of data to each stream evenly.  Writing too much
   data to one stream and not enough to another stream may result in the
   underlying movie writer's buffer for that stream overflowing,
   causing an error.

Side Effects: 
   As a result of this call, data may or may not be written to the movie file.
   This is because there is an internal buffer used to combine writes together
   and to perform stream interleaving.

See Also:
   AEEMovieFileInfo, IMOVIEFILE_Open(), IMOVIEFILE_WriteUserAtom(),
   IMOVIEFILE_WriteStream().

=============================================================================

Function:  IMOVIEFILE_ReadSegmentHeader()

Description:
   This function is used to read one of the headers of a segment of a stream
   in the movie file meta-data.  These headers become available for reading
   as soon as the file is opened for reading using IMOVIEFILE_Open(), and
   remain available even after the streams in the file are read using
   IMOVIEFILE_ReadStream().

   The movie file must be an existing movie file opened for reading.
   See IMOVIEFILE_Open() for more details.

   A segment header for video/audio is referred to in the MP4 file format
   as "decoder specific information".  For timed text, the segment header is
   a 'tx3g' atom containing default values for the text renderer.

Prototype:
   int IMOVIEFILE_ReadSegmentHeader (IMovieFile * po, uint16 nStream,
                                     uint16 nSegment,
                                     void * pBuffer, uint32 * pnSize)

Parameters:
   po [in]:                Pointer to the IMovieFile interface object
   uint16 nStream [in]:    This is the index of the stream to read.  The index
                           is used to look up the stream in the array in
                           the AEEMovieFileInfo structure given to the user
                           when IMOVIEFILE_Open() is called.
   uint16 nSegment [in]:   Sequence number of the segment in the stream.
                           The first segment in the stream has a sequence
                           number of zero, the next segment headerafter that
                           has a sequence number of one, and so on.
   void * pBuffer [out]:   This is a pointer to where the segment header data
                           should be stored.  If NULL, no data is actually
                           stored.
   uint32 * pnSize [out]:  This is where the total size of the header in bytes
                           is stored.  If this is set to zero, then there is
                           no segment header with the given sequence number.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   The user must be sure to allocate enough space in the buffer for storing
   the data from the segment header.  To determine the amount of space
   required, the user can call the function with NULL for the buffer pointer,
   in which case the data is not stored but the total size of the header
   is still set in the variable pointed to by pnSize.

   The user can determine the number of segment headers in the stream
   by reading with sequence numbers starting at zero and increasing until
   the returned header size is zero, indicating there are no more segment
   headers in the stream.

   For video and audio streams, the data buffer for the segment header will
   contain the "decoder specific information", typically from the 'esds' atom
   for that segment of the stream.  For timed text, the data buffer will
   contain the entire contents of 'tx3g' atom, excluding the atom size and
   type fields (the first 8 bytes).

Side Effects: 
   None.

See Also:
   IMOVIEFILE_Open(), IMOVIEFILE_ReadStream().

=============================================================================

Function:  IMOVIEFILE_WriteSegmentHeader()

Description:
   This function is used to write a raw segment header for a stream in the
   movie file meta-data.  These can be written anytime after calling
   IMOVIEFILE_Open().

   The movie file must be a new movie file created for writing.
   See IMOVIEFILE_Open() for more details.

   A segment header for video/audio is referred to in the MP4 file format
   as "decoder specific information".  For timed text, the segment header is
   a 'tx3g' atom containing default values for the text renderer.

   Do not call this function to write the segment header for video/audio.
   Instead, put the segment header in the first sample of the stream, in
   front of the sample itself.  Currently, writing more than one segment
   header for a video/audio stream is not supported.

   For a timed text stream, the segment header should contain the raw contents
   of a 'tx3g' atom, excluding the atom size and type fields (first 8 bytes).
   These fields are automatically added by the movie file writer.

   Once a segment header is written, it applies to all samples written to the
   stream from that point on, until the end of the stream is reached or the
   next segment header is written.

Prototype:
   int IMOVIEFILE_WriteSegmentHeader (IMovieFile * po, uint16 nStream,
                                      const void * pBuffer, uint32 dwSize)

Parameters:
   po [in]:                    Pointer to the IMovieFile interface object
   uint16 nStream [in]:        This is the index of the stream to
                               write.  The index is used to look up
                               the stream in the array in the
                               AEEMovieFileInfo structure given to
                               the user when IMOVIEFILE_Open() is called.
   const void * pBuffer [in]:  This points to the data to be written out
                               to the segment header.
   uint32 dwSize [in]:         This is the size of the header to write out.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments: 
   For video and audio streams, the data buffer for the segment header will
   contain the "decoder specific information", typically from the 'esds' atom
   for that segment of the stream.  For timed text, the data buffer will
   contain the entire contents of 'tx3g' atom, excluding the atom size and
   type fields (the first 8 bytes).

   Segment headers written using this method are written sequentially to
   the movie file, and cause all samples written afterward to be associated
   with that segment.  Writing segment headers in a random access fashion
   is not supported.

Side Effects: 
   None.

See Also:
   IMOVIEFILE_Open(), IMOVIEFILE_WriteStream().

=========================================================================== */

#endif // AEEMOVIEFILE_H
