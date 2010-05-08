#ifndef AEEMIMETYPES_H
#define AEEMIMETYPES_H
/*======================================================
FILE:  AEEMimeTypes.h

SERVICES:  AEE Mime Type Definitions

GENERAL DESCRIPTION:
	Base level definitions, typedefs, etc. for AEE

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/


// 
// MIME base prefixes
//

#define IMAGE_MIME_BASE    "image/"
#define SOUND_MIME_BASE    "snd/"
#define AUDIO_MIME_BASE    "audio/"
#define VIDEO_MIME_BASE    "video/"

#define WAP_EXT            "wml"
#define WEB_EXT            "htm"
#define MIDI_EXTENSION     "mid"
#define MP3_EXTENSION      "mp3"
#define QCP_EXTENSION      "qcp"
#define VNDQCP_EXTENSION   "vnd.qcelp"
#define QCF_EXTENSION      "qcf"
#define MPEG4_EXTENSION    "mp4"
#define MMF_EXTENSION      "mmf"
#define PHR_EXTENSION      "spf"
#define IMY_EXTENSION      "imy"
#define ADPCM_EXTENSION    "wav"
#define AAC_EXTENSION      "aac"
#define AMR_EXTENSION      "amr"
#define WMA_EXTENSION      "wma"
#define HVS_EXTENSION      "hvs"
#define SAF_EXTENSION      "saf"

#define MT_MIDI            "snd/midi"
#define MT_MP3             "snd/mp3"
#define MT_QCP             "snd/qcp"
#define MT_VNDQCP          "snd/vnd.qcelp"
#define MT_QCF             "snd/qcf"
#define MT_MMF             "snd/mmf"
#define MT_PHR             "snd/spf"
#define MT_IMELODY         "snd/imy"
#define MT_ADPCM           "snd/wav"
#define MT_AAC             "snd/aac"
#define MT_AMR             "snd/amr"
#define MT_WMA             "snd/wma"
#define MT_BMP             "image/bmp"
#define MT_BCI             "image/bci"
#define MT_GIF             "image/gif"
#define MT_PNG             "image/png"
#define MT_JPEG            "image/jpeg"
#define MT_JPG             "image/jpg"

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
#define MT_AUDIO_MXMF      "audio/mxmf"
#define MT_AUDIO_XMF0      "audio/xmf0"
#define MT_AUDIO_XMF1      "audio/xmf1"
#define MT_AUDIO_DLS       "audio/dls"

#define MT_VIDEO_PMD       "video/pmd"
#define MT_VIDEO_MPEG4     "video/mp4"
#define MT_VIDEO_WMV       "video/wmv"
#define MT_VIDEO_SVG       "video/svg"
#define MT_VIDEO_SVGZ      "video/svgz"

// Internal mime-types

#define MT_SCREEN_SAVER    "brew/ssaver"     // Must register with AEECLSID_SCREEN_SAVER

// encoding MIME-types
#define USASCII		"US-ASCII"
#define UTF8		"UTF-8"
#define UTF16		"UTF-16"
#define ISO88591	"ISO-8859-1"

#endif      // AEEMIMETYPES_H

/*=============================================================================
  DATA STRUCTURE DOCUMENTATION
===============================================================================
AEEMIMETypes

Description:

The following defines the mime-types.

Definition:

MIME base prefixes

#define IMAGE_MIME_BASE    "image/"
#define SOUND_MIME_BASE    "snd/"
#define AUDIO_MIME_BASE    "audio/"
#define VIDEO_MIME_BASE    "video/"

#define WAP_EXT            "wml"
#define WEB_EXT            "htm"
#define MIDI_EXTENSION     "mid"
#define MP3_EXTENSION      "mp3"
#define QCP_EXTENSION      "qcp"
#define MPEG4_EXTENSION    "mp4"
#define MMF_EXTENSION      "mmf"
#define PHR_EXTENSION      "spf"
#define ADPCM_EXTENSION    "wav"
#define AAC_EXTENSION      "aac"
#define HVS_EXTENSION      "hvs"
#define SAF_EXTENSION      "saf"

// Sound mime-types

#define MT_MIDI            "snd/midi"
#define MT_MP3             "snd/mp3"
#define MT_QCP             "snd/qcp"

// Image mime-types

#define MT_BMP             "image/bmp"
#define MT_BCI             "image/bci"
#define MT_GIF             "image/gif"
#define MT_PNG             "image/png"
#define MT_JPEG            "image/jpeg"
#define MT_JPG             "image/jpg"

// Audio mime-types

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
#define MT_AUDIO_HVS       "audio/hvs"
#define MT_AUDIO_SAF       "audio/saf"

// Video mime-types

#define MT_VIDEO_PMD       "video/pmd"
#define MT_VIDEO_MPEG4     "video/mp4"

// Internal mime-types

#define MT_SCREEN_SAVER    "brew/ssaver"     // Must register with AEECLSID_SCREEN_SAVER

// Encoding MIME-types

#define USASCII		"US-ASCII"
#define UTF8		"UTF-8"
#define UTF16		"UTF-16"
#define ISO88591	"ISO-8859-1"

=============================================================================*/

