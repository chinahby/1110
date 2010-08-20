#ifndef MGMEDIAINFO_H
#define MGMEDIAINFO_H
/*
 * =====================================================================================
 *
 *       Filename:  MGMediaInfo.h
 *
 *    Description:  Get media information from file.
 *
 *        Version:  1.0
 *        Created:  2008-11-19 14:16:20
 *       Revision:  none
 *       Compiler:  ADS12
 *
 * =====================================================================================
 */
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "AEEMimeTypes.h"
#include "AEEMedia.h"
#include "OEMImageParmExt.h"
#include "Jpeg.bid"
#include "MediaGalleryExplorer.h"
#include "AEEClipboard.h"

/*files extension list */
//types of Image Files
#define  MG_BMP_EXTENSION            "bmp"
#define  MG_GIF_EXTENSION            "gif"
#define  MG_PNG_EXTENSION            "png"
#define  MG_BCI_EXTENSION            "bci"
#define  MG_JPG_EXTENSION            "jpg"
#define  MG_JPEG_EXTENSION           "jpeg"

//types of Sound Files
#define MG_AAC_EXTENSION        AAC_EXTENSION
#define MG_AMR_EXTENSION	AMR_EXTENSION
#define MG_IMY_EXTENSION	IMY_EXTENSION
#define MG_MIDI_EXTENSION	MIDI_EXTENSION
#define MG_MMF_EXTENSION	MMF_EXTENSION
#define MG_MP3_EXTENSION	MP3_EXTENSION
#define MG_PHR_EXTENSION	PHR_EXTENSION
#define MG_QCF_EXTENSION	QCF_EXTENSION
#define MG_QCP_EXTENSION	QCP_EXTENSION
#define MG_WMA_EXTENSION	WMA_EXTENSION
#define MG_WAV_EXTENSION        ADPCM_EXTENSION
#define MG_HVS_EXTENSION         HVS_EXTENSION
#define MG_SAF_EXTENSION         SAF_EXTENSION

//types of video files
#define MG_MPEG4_EXTENSION       MPEG4_EXTENSION
#define MG_WMV_EXTENSION         "wmv"
#define MG_PMD_EXTENSION         "pmd"
#define MG_SVG_EXTENSION         "svg"
#define MG_SVGZ_EXTENSION        "svgz"
#define MG_3GP_EXTENSION         "3gp"
#define MG_3GPP_EXTENSION        "3gpp"

/*file extension list end*/
#define MG_MT_UNKNOW             "bar/foo"

typedef enum _MGMimeType{
   MG_MIME_UNKNOWN = 0x00,
   MG_MIME_MISCELL,/*various media file, for ex. MMC*/
   MG_MIME_MISMAX,

   /*IMG: image*/
   MG_MIME_IMGBASE = 0x10,
   MG_MIME_BCI,
   MG_MIME_BMP,
   MG_MIME_GIF,
   MG_MIME_JPEG,
   MG_MIME_JPG,
   MG_MIME_PNG,
   MG_MIME_IMGMAX,

   /*snd: sound*/
   MG_MIME_SNDBASE = 0x30,
   //MG_MIME_AAC,//AAC, AAC+
   //MG_MIME_AMR,
   MG_MIME_MIDI,
   MG_MIME_MP3,
   //MG_MIME_WAV,
   //MG_MIME_WMA,
   //MG_MIME_DLS,
   //MG_MIME_HVS,
   //MG_MIME_IMELODY,
   //MG_MIME_MMF,
   //MG_MIME_MXMF,
   //MG_MIME_PHR,
   //MG_MIME_QCF,
   MG_MIME_QCP,
#ifdef FEATURE_AAC
   MG_MIME_AAC,
#endif
   //MG_MIME_SAF,
   //MG_MIME_XMF,
   //MG_MIME_XMF0,
   //MG_MIME_XMF1,
   MG_MIME_SNDMAX,

   MG_MIME_VIDEOBASE = 0x50,
   MG_MIME_3GP,
   MG_MIME_MP4,
   MG_MIME_PMD,
   MG_MIME_SVG,
   MG_MIME_SVGZ,
   MG_MIME_WMV,
   MG_MIME_VIDEOMAX,

   MG_MIME_VOICEREC,
   MG_MIME_CALLREC,

   MG_MIME_FOLDER,

   MG_MIME_MAX = 0XFF,
   MG_MIME_INVALID = MG_MIME_MAX,
}MGMimeType;

typedef struct MGMimeMap{
   const char* pszMime;
   MGMimeType  MimeType;
}MGMimeMap;

typedef struct _MGExt2Mime{
   char*       pszFileExt;
   MGMimeType  eMime;
}MGExt2Mime;

typedef enum MGTagType{
   MGTAG_NOTAG,//Empty set
   MGTAG_ID3V1,//ID3v1 & ID3v1.1
   MGTAG_ID3V2,//ID3v2 series, latest is 2.4.0
   MGTAG_ALLTAG,//Have ID3V1 & ID3V2
}MGTagType;

/**
* The four types of string encodings supported by the ID3v2 specification.
* ( refer to chapter "ID3v2 frame overview " in id3v2.4.0-structure.txt)
* ID3v1 is assumed to be Latin1 and Ogg Vorbis comments use UTF8.
*/
typedef enum EncType {
   /* IS08859-1, or Latin1 encoding.  8 bit characters.  */
   ENC_LATIN1 = 0,
   /* UTF16 with a byte order mark.  16 bit characters.  */
   ENC_UTF16 = 1,
   /* UTF16 big endian.  16 bit characters.  */
   ENC_UTF16BE = 2,
   /* UTF8 encoding.  Characters are usually 8 bits but can be up to 32.  */
   ENC_UTF8 = 3,
   /* UTF16 little endian.  16 bit characters.  */
   ENC_UTF16LE = 4
}EncType;

#define MGINFO_ID3V1_NONETRACK 0xFFFF
typedef struct MGInfo_ID3V1{
   char m_title[30];
   char m_artist[30];
   char m_album[30];
   char m_year[4];
   char m_comment[30];
   uint16 m_track;
   uint8  m_genre;
}MGInfo_ID3V1;

/*ID3V2 Frame ID list,four characters
 * The frame ID made out of the characters capital A-Z and 0-9.*/
/*Year*/
#define MGINFO_ID3V2FID_YEAR     "TYER"
/*comment*/
#define MGINFO_ID3V2FID_COMM     "COMM"
/**************Identification frames************/
/*Title/Songname/Content description frame, the actual name of the song*/
#define MGINFO_ID3V2FID_TITLE    "TIT2"
/*Album/Movie/Show title*/
#define MGINFO_ID3V2FID_ALBUM    "TALB"
/*track*/
#define MGINFO_ID3V2FID_TRCK     "TRCK"
/*Lead artist/Lead performer/Soloist/Performing group, used for the main artist.*/
#define MGINFO_ID3V2FID_ARTIST      "TPE1"
/*Derived and subjective properties frames*/
#define MGINFO_ID3V2FID_TCON     "TCON"


__inline int MGMediaInfo_MimeBase2FileType(MGMimeType eMimeBase)
{
   switch(eMimeBase)
   {
      case MG_MIME_IMGBASE:
         return MGMIME_BASE_IMAGE;

      case MG_MIME_SNDBASE:
      case MG_MIME_CALLREC:
      case MG_MIME_VOICEREC:
         return MGMIME_BASE_MUSIC;

      case MG_MIME_VIDEOBASE:
         return MGMIME_BASE_VIDEO;

      default:
         break;
   }

   return MGMIME_BASE_ALL;
}//MGMediaInfo_MimeBaseConvert

__inline int MGMediaInfo_MimeBaseConvert(MGMimeType eMimeBase)
{
   return MGMediaInfo_MimeBase2FileType(eMimeBase);
}

__inline uint16 MGMediaInfo_FileType2MimeBase(uint16 nFileType)
{
   switch(nFileType)
   {
   case MGMIME_BASE_IMAGE:
      return MG_MIME_IMGBASE;

   case MGMIME_BASE_MUSIC:
      return MG_MIME_SNDBASE;

   case MGMIME_BASE_VIDEO:
      return MG_MIME_VIDEOBASE;

   default:
      break;
   }

   return MG_MIME_MISCELL;
}//MGMediaInfo_FileType2MimeBase


/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
int MGMediaInfo_GetMimeType(IShell* pShell,
                           IFileMgr*  pFileMgr,
                           const char* pszFilePath,
                           uint16 MimeBase,
                           MGMimeType* peMime);

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION: check eItmeMime whether belong to eBaseMime range
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
boolean MGMediaInof_ValidateMime(MGMimeType eBaseMime, MGMimeType eItmeMime);

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
uint32 MGMediaInfo_GetID3V1(IFile* pIFile, MGInfo_ID3V1* pTag);

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
uint32 MGMediaInfo_GetID3V2(IFile* pIFile);

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
boolean MGMediaInfo_GetMP3Genre(byte nGenre, const AECHAR *pGenreStr);
#endif//MGMEDIAINFO_H
