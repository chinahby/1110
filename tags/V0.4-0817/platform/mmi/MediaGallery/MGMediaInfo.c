/*
 * =====================================================================================
 *
 *       Filename:  MGMediaInfo.c
 *
 *    Description:  Get media information from file.
 *
 *        Version:  1.0
 *        Created:  2008-11-19 14:20:00
 *       Revision:  create.
 *       Compiler:  ADS12
 *
 *        Company:  JRDCOM
 *
 * =====================================================================================
 */
#include "MediaGalleryApp.h"
#include "MediaGalleryExplorer.h"
#include "MGMediaInfo.h"
#include "Msg.h"

static int MGMediaInfo_DetectImgType(IFile *  pIFile,
                           MGMimeType* peMime);
static int MGMediaInfo_DetectSndVideoType(IShell* pShell,
                        IFile* pIFile,
                        uint32 MimeBase,
                        MGMimeType* peMime);
/*===========================================================================
 *
 *                              LOCAL/STATIC DATA
 *
 *============================================================================
*/


/*MT_MIDI, MT_MP3 seem to be deprecate,*/
const static MGMimeMap  SoundMimeMap[]={
        // {MT_AUDIO_AAC, MG_MIME_AAC},     {MT_AUDIO_ADPCM, MG_MIME_WAV},
        // {MT_AUDIO_AMR, MG_MIME_AMR},     {MT_AUDIO_DLS, MG_MIME_DLS},
        // {MT_AUDIO_HVS, MG_MIME_HVS},     {MT_AUDIO_IMELODY, MG_MIME_IMELODY},
         {MT_AUDIO_MIDI, MG_MIME_MIDI},  // {MT_AUDIO_MMF, MG_MIME_MMF},
         {MT_AUDIO_MP3, MG_MIME_MP3},  //   {MT_AUDIO_MXMF, MG_MIME_MXMF},
       //  {MT_AUDIO_PHR, MG_MIME_PHR},     {MT_AUDIO_QCF, MG_MIME_QCF},
         {MT_AUDIO_QCP, MG_MIME_QCP},     //{MT_AUDIO_SAF, MG_MIME_SAF},
       //  {MT_AUDIO_WMA, MG_MIME_WMA},     {MT_AUDIO_XMF, MG_MIME_XMF},
       //  {MT_AUDIO_XMF0, MG_MIME_XMF0},   {MT_AUDIO_XMF1, MG_MIME_XMF1},
};

const static MGMimeMap ImageMimeMap[] = {
         {MT_PNG, MG_MIME_PNG},  {MT_BMP, MG_MIME_BMP},
         {MT_GIF, MG_MIME_GIF},  {MT_JPEG, MG_MIME_JPG},
         {MT_JPG, MG_MIME_JPG},  {MT_BCI, MG_MIME_BCI},
};

const static MGMimeMap VideoMimeMap[] = {
/*3gpp need to check, BREW do not support for default.*/
         {MT_VIDEO_MPEG4, MG_MIME_MP4},   {MT_VIDEO_PMD, MG_MIME_PMD},
         {MT_VIDEO_SVG, MG_MIME_SVG},     {MT_VIDEO_SVGZ, MG_MIME_SVGZ},
         {MT_VIDEO_WMV, MG_MIME_WMV},
};

const static MGExt2Mime ImageFileExt[] = {
         /*image file extension*/
         {MG_BMP_EXTENSION, MG_MIME_BMP}, {MG_PNG_EXTENSION, MG_MIME_PNG},
         {MG_JPG_EXTENSION, MG_MIME_JPG}, {MG_JPEG_EXTENSION, MG_MIME_JPEG},
         {MG_GIF_EXTENSION, MG_MIME_GIF}, {MG_BCI_EXTENSION, MG_MIME_BCI},
};

const static MGExt2Mime SoundFileExt[] = {
         /*sound file extension*/
        /* {MG_AAC_EXTENSION, MG_MIME_AAC}, */{MG_MP3_EXTENSION, MG_MIME_MP3},
        /* {MG_WAV_EXTENSION, MG_MIME_WAV}, */{MG_MIDI_EXTENSION, MG_MIME_MIDI},
        /* {MG_WMA_EXTENSION, MG_MIME_WMA},*/{MG_QCP_EXTENSION, MG_MIME_QCP},
};

const static MGExt2Mime VideoFileExt[] = {
         /*video file extension*/
         {MG_3GP_EXTENSION, MG_MIME_3GP}, {MG_MPEG4_EXTENSION, MG_MIME_MP4},
         {MG_WMV_EXTENSION, MG_MIME_WMV}, {MG_PMD_EXTENSION, MG_MIME_PMD},
         {MG_SVG_EXTENSION, MG_MIME_SVG}, {MG_SVGZ_EXTENSION, MG_MIME_SVGZ}
      };

/*file header for image, video and audio file refer to function
 * OEMMedia_DetectType in OMEMedia.c*/
#define MG_IMGFILEHEAD_LEN 16
const static byte cbFMT_JPEG[] = {0xFF, 0xD8, 0xFF, 0xE0};
const static byte cbFMT_PNG[] = {0x89, 'P', 'N', 'G'};/*0x89,0x50,0x4E,0x47*/
const static byte cbFMT_BMP[] = {'B','M'};/*0x42 0x40*/
const static byte cbFMT_BCI[] = {'B','C','I'};/*0x42,0x43,0x49*/
const static byte cbFMT_GIF87A[] = {'G','I','F','8','7','a'};
const static byte cbFMT_GIF89A[] = {'G','I','F','8','9','a'};

#define MG_MIME_LEN        32
#define MEDIAINFO_ID3V1_TAG          "TAG"
#define ID3V1_LEN          128

//"ID3"
#define MEDIAINFO_IS_ID3V2_TAG(x) \
            (((byte *)(x))[0] == 0x49 && \
             ((byte *)(x))[1] == 0x44 && \
             ((byte *)(x))[2] == 0x33)

#define MGINFO_ID3V2_HEADERLEN   10
#define MGINFO_ID3V2_FOOTERLEN   10
/*ID3v2 size    4 * %0xxxxxxx*/
#define ID3V2_HEADERLEN_BYTENUM     4
#define ID3V2_HEADERLEN_BYTEPOWER   7
#define ID3V2_HEADERFLAG_FOOTERMASK 0x10

static const AECHAR* ID3V1_Genres[]={
   /*0*/   L"Blues",
   /*1*/   L"Classic Rock",
   /*2*/   L"Country",
   /*3*/   L"Dance",
   /*4*/   L"Disco",
   /*5*/   L"Funk",
   /*6*/   L"Grunge",
   /*7*/   L"Hip-Hop",
   /*8*/   L"Jazz",
   /*9*/   L"Metal",
   /*10*/   L"New Age",
   /*11*/   L"Oldies",
   /*12*/   L"Other",
   /*13*/   L"Pop",
   /*14*/   L"R&B",
   /*15*/   L"Rap",
   /*16*/   L"Reggae",
   /*17*/   L"Rock",
   /*18*/   L"Techno",
   /*19*/   L"Industrial",
   /*20*/   L"Alternative",
   /*21*/   L"Ska",
   /*22*/   L"Death Metal",
   /*23*/   L"Pranks",
   /*24*/   L"Soundtrack",
   /*25*/   L"Euro-Techno",
   /*26*/   L"Ambient",
   /*27*/   L"Trip-Hop",
   /*28*/   L"Vocal",
   /*29*/   L"Jazz+Funk",
   /*30*/   L"Fusion",
   /*31*/   L"Trance",
   /*32*/   L"Classical",
   /*33*/   L"Instrumental",
   /*34*/   L"Acid",
   /*35*/   L"House",
   /*36*/   L"Game",
   /*37*/   L"Sound Clip",
   /*38*/   L"Gospel",
   /*39*/   L"Noise",
   /*40*/   L"Alternative Rock",
   /*41*/   L"Bass",
   /*42*/   L"Soul",
   /*43*/   L"Punk",
   /*44*/   L"Space",
   /*45*/   L"Meditative",
   /*46*/   L"Instrumental Pop",
   /*47*/   L"Instrumental Rock",
   /*48*/   L"Ethnic",
   /*49*/   L"Gothic",
   /*50*/   L"Darkwave",
   /*51*/   L"Techno-Industrial",
   /*52*/   L"Electronic",
   /*53*/   L"Pop-Folk",
   /*54*/   L"Eurodance",
   /*55*/   L"Dream",
   /*56*/   L"Southern Rock",
   /*57*/   L"Comedy",
   /*58*/   L"Cult",
   /*59*/   L"Gangsta",
   /*60*/   L"Top 40",
   /*61*/   L"Christian Rap",
   /*62*/   L"Pop/Funk",
   /*63*/   L"Jungle",
   /*64*/   L"Native American",
   /*65*/   L"Cabaret",
   /*66*/   L"New Wave",
   /*67*/   L"Psychedelic",
   /*68*/   L"Rave",
   /*69*/   L"Showtunes",
   /*70*/   L"Trailer",
   /*71*/   L"Lo-Fi",
   /*72*/   L"Tribal",
   /*73*/   L"Acid Punk",
   /*74*/   L"Acid Jazz",
   /*75*/   L"Polka",
   /*76*/   L"Retro",
   /*77*/   L"Musical",
   /*78*/   L"Rock & Roll",
   /*79*/   L"Hard Rock",
   /*80*/   L"Folk",
   /*81*/   L"Folk/Rock",
   /*82*/   L"National Folk",
   /*83*/   L"Swing",
   /*84*/   L"Fusion",
   /*85*/   L"Bebob",
   /*86*/   L"Latin",
   /*87*/   L"Revival",
   /*88*/   L"Celtic",
   /*89*/   L"Bluegrass",
   /*90*/   L"Avantgarde",
   /*91*/   L"Gothic Rock",
   /*92*/   L"Progressive Rock",
   /*93*/   L"Psychedelic Rock",
   /*94*/   L"Symphonic Rock",
   /*95*/   L"Slow Rock",
   /*96*/   L"Big Band",
   /*97*/   L"Chorus",
   /*98*/   L"Easy Listening",
   /*99*/   L"Acoustic",
   /*100*/   L"Humour",
   /*101*/   L"Speech",
   /*102*/   L"Chanson",
   /*103*/   L"Opera",
   /*104*/   L"Chamber Music",
   /*105*/   L"Sonata",
   /*106*/   L"Symphony",
   /*107*/   L"Booty Bass",
   /*108*/   L"Primus",
   /*109*/   L"Porn Groove",
   /*110*/   L"Satire",
   /*111*/   L"Slow Jam",
   /*112*/   L"Club",
   /*113*/   L"Tango",
   /*114*/   L"Samba",
   /*115*/   L"Folklore",
   /*116*/   L"Ballad",
   /*117*/   L"Power Ballad",
   /*118*/   L"Rhythmic Soul",
   /*119*/   L"Freestyle",
   /*120*/   L"Duet",
   /*121*/   L"Punk Rock",
   /*122*/   L"Drum Solo",
   /*123*/   L"A Cappella",
   /*124*/   L"Euro-House",
   /*125*/   L"Dance Hall",
   /*126*/   L"Goa",
   /*127*/   L"Drum & Bass",
   /*128*/   L"Club-House",
   /*129*/   L"Hardcore",
   /*130*/   L"Terror",
   /*131*/   L"Indie",
   /*132*/   L"BritPop",
   /*133*/   L"Negerpunk",
   /*134*/   L"Polsk Punk",
   /*135*/   L"Beat",
   /*136*/   L"Christian Gangsta Rap",
   /*137*/   L"Heavy Metal",
   /*138*/   L"Black Metal",
   /*139*/   L"Crossover",
   /*140*/   L"Contemporary Christian",
   /*141*/   L"Christian Rock",
   /*142*/   L"Merengue",
   /*143*/   L"Salsa",
   /*144*/   L"Thrash Metal",
   /*145*/   L"Anime",
   /*146*/   L"Jpop",
   /*147*/   L"Synthpop"
};


/*convert MIME to MGMimeType*/
int MGMediaInfo_MimeStrToType(const char* pszMime, uint32 MimeBase, MGMimeType* peMime)
{
   uint16 i;
   size_t nLen = sizeof(char) * STRLEN(pszMime);

   if(!pszMime || !peMime)
   {
      return EBADPARM;
   }

   if(MimeBase & MGMIME_BASE_IMAGE)
   {
      for( i = 0; i < ARRAYSIZE(ImageMimeMap); i++)
      {
         if(0 == STRNCMP(pszMime, ImageMimeMap[i].pszMime, nLen))
         {
            *peMime = ImageMimeMap[i].MimeType;
            return SUCCESS;
         }
      }
   }

   if(MimeBase & MGMIME_BASE_MUSIC)
   {
      for( i = 0; i < ARRAYSIZE(SoundMimeMap); i++)
      {
         if(0 == STRNCMP(pszMime, SoundMimeMap[i].pszMime, nLen))
         {
            *peMime = SoundMimeMap[i].MimeType;
            return SUCCESS;
         }
      }
   }

   if(MimeBase & MGMIME_BASE_VIDEO)
   {
      for( i = 0; i < ARRAYSIZE(VideoMimeMap); i++)
      {
         if(0 == STRNCMP(pszMime, VideoMimeMap[i].pszMime, nLen))
         {
            *peMime = VideoMimeMap[i].MimeType;
            return SUCCESS;
         }
      }
   }

   return EFAILED;
}//MGMediaInfo_MimeStrToType

/*convert MGMimeType to MIME */
int MGMediaInof_MimeTypeToStr(const MGMimeType eMime,  const char** ppszMime)
{
   uint16 i;
   uint16 nSize;
   const MGMimeMap* pMimeMapTable;

   if(!ppszMime)
   {
      return EBADPARM;
   }

   if(eMime < MG_MIME_IMGBASE)
   {
      *ppszMime = MG_MT_UNKNOW;
      return SUCCESS;
   }
   else if(eMime < MG_MIME_IMGMAX)
   {
      nSize = ARRAYSIZE(ImageMimeMap);
      pMimeMapTable = ImageMimeMap;
   }
   else if(eMime < MG_MIME_SNDMAX)
   {
      nSize = ARRAYSIZE(SoundMimeMap);
      pMimeMapTable = SoundMimeMap;
   }
   else if(eMime < MG_MIME_VIDEOMAX)
   {
      nSize = ARRAYSIZE(VideoMimeMap);
      pMimeMapTable = VideoMimeMap;
   }
   else
   {
      return EFAILED;
   }

   for(i = 0; i < nSize; i++)
   {
      if(eMime == pMimeMapTable[i].MimeType)
      {
         *ppszMime = pMimeMapTable[i].pszMime;
         return SUCCESS;
      }
   }

   return EFAILED;
}//MGMediaInof_MimeTypeToStr


/*===========================================================================
 * FUNCTION:MGMediaInfo_GetMimeType
 * DESCRIPTION:  find out the MIME type for the giving file extension
 * PARAMETERS:
 *  nLen:[in] buffer length in byte
 * RETURN VALUE:
 *    SUCCESS: we can return some thing as expect.
 *    EFAILED: Parameter error, or parameter is not valid.
 *===========================================================================
 */
int MGMediaInfo_GetMimeType(IShell* pShell,
                           IFileMgr*  pFileMgr,
                           const char* pszFilePath,
                           uint16 MimeBase,
                           MGMimeType* peMime)
{
   uint16 i;
   size_t nLen;
   char*  pszExt;

   if(!pszFilePath || !peMime)
   {
      return EBADPARM;
   }
   MSG_FATAL("MGMediaInfo_GetMimeType Start",0,0,0);
   // If it is file name, then first check the extension.
   // If there is no extension, then read the file header data and
   // analyze it to determine its MIME

   // Check the extension...
   pszExt = STRRCHR(pszFilePath, MG_DOT_CHAR);
   if (pszExt)
      pszExt++;

   if (pszExt && *pszExt)
   {
      MSG_FATAL("pszExt && *pszExt" ,0,0,0);
      nLen = STRLEN(pszExt)*sizeof(char);

      if(MimeBase & MGMIME_BASE_IMAGE)
      {
         for( i = 0; i< ARRAYSIZE(ImageFileExt); i++)
         {
            /*ignore capital*/
            if( 0 == STRNICMP(pszExt, ImageFileExt[i].pszFileExt, nLen))
            {
               *peMime = ImageFileExt[i].eMime;
               MSG_FATAL("peMime = " ,ImageFileExt[i].eMime,0,0);
               return SUCCESS;
            }
         }
      }

      if(MimeBase & MGMIME_BASE_MUSIC)
      {
         for( i = 0; i< ARRAYSIZE(SoundFileExt); i++)
         {
            /*ignore capital*/
            if( 0 == STRNICMP(pszExt, SoundFileExt[i].pszFileExt, nLen))
            {
               *peMime = SoundFileExt[i].eMime;
               return SUCCESS;
            }
         }
      }

      if(MimeBase & MGMIME_BASE_VIDEO)
      {
         for( i = 0; i< ARRAYSIZE(VideoFileExt); i++)
         {
            /*ignore capital*/
            if( 0 == STRNICMP(pszExt, VideoFileExt[i].pszFileExt, nLen))
            {
               *peMime = VideoFileExt[i].eMime;
               return SUCCESS;
            }
         }
      }
   }
   else
   {
      // Now, read the file contents and try to determine the format...
      IFileMgr* pfm;
      IFile*    pIFile;
      IShell*   ps = pShell;
      int      nRet = EFAILED;
      MSG_FATAL("STRRCHR Failed" ,0,0,0);
      if (!ps)
         ps = AEE_GetShell();

      if (pFileMgr)
         pfm = pFileMgr;
      else if (ISHELL_CreateInstance(ps, AEECLSID_FILEMGR, (void **)&pfm))
         return ENOMEMORY;

      pIFile = IFILEMGR_OpenFile(pfm, pszFilePath, _OFM_READ);
      if (!pIFile)
      {
         if (!pFileMgr)
            RELEASEIF(pfm);
         return ENOMEMORY;
      }

      if(MimeBase & MGMIME_BASE_IMAGE)
         nRet = MGMediaInfo_DetectImgType(pIFile, peMime);

      if(((MimeBase & MGMIME_BASE_MUSIC) || (MimeBase & MGMIME_BASE_VIDEO))
            &&(SUCCESS != nRet))
         nRet = MGMediaInfo_DetectSndVideoType(ps, pIFile, MimeBase, peMime);

      RELEASEIF(pIFile);
      if (!pFileMgr)
         RELEASEIF(pfm);

      if(SUCCESS == nRet)
         return SUCCESS;
   }

   *peMime = MG_MIME_UNKNOWN;

   MSG_FATAL("MGMediaInfo_GetMimeType End GetMimeType failed!",0,0,0);
   return EFAILED;
}//MGMediaInfo_GetMimeType

static int MGMediaInfo_DetectImgType(IFile *  pIFile,
                           MGMimeType* peMime)
{
   int32  nRead;
   byte   Buf[MG_IMGFILEHEAD_LEN];

   if(!pIFile || !peMime)
      return EFAILED;

   MEMSET(Buf, 0, sizeof(Buf));
   nRead = IFILE_Read(pIFile, Buf, sizeof(Buf));

   if(nRead == 0)
      return EFAILED;

   if(0 == MEMCMP(Buf, cbFMT_JPEG, sizeof(cbFMT_JPEG)))
   {
      *peMime = MG_MIME_JPEG;
      return SUCCESS;
   }

   if(0 == MEMCMP(Buf, cbFMT_PNG, sizeof(cbFMT_PNG)))
   {
      *peMime = MG_MIME_PNG;
      return SUCCESS;
   }

   if(0 == MEMCMP(Buf, cbFMT_BMP, sizeof(cbFMT_BMP)))
   {
      *peMime = MG_MIME_BMP;
      return SUCCESS;
   }

   if(0 == MEMCMP(Buf, cbFMT_BCI, sizeof(cbFMT_BCI)))
   {
      *peMime = MG_MIME_BCI;
      return SUCCESS;
   }

   if(0 == MEMCMP(Buf, cbFMT_GIF87A, sizeof(cbFMT_GIF87A)))
   {
      *peMime = MG_MIME_GIF;
      return SUCCESS;
   }

   if(0 == MEMCMP(Buf, cbFMT_GIF89A, sizeof(cbFMT_GIF89A)))
   {
      *peMime = MG_MIME_GIF;
      return SUCCESS;
   }

   return EFAILED;
}//MGMediaInfo_DetectImgType

static int MGMediaInfo_DetectSndVideoType(IShell* pShell,
                        IFile* pIFile,
                        uint32 MimeBase,
                        MGMimeType* peMime)
{
   void *      pBuf = NULL;
   uint32      dwReqSize = 0;
   const char *cpszMIME;
   int         nErr = SUCCESS;
   int         nRead;
   IShell*     ps = pShell;

   if(!peMime || !pIFile)
      return EBADPARM;

   if(!ps)
      ps = AEE_GetShell();

   if (ENEEDMORE != ISHELL_DetectType(ps, NULL, &dwReqSize, NULL, NULL) || !dwReqSize)
      return EFAILED;

   pBuf = MALLOC(dwReqSize);
   if (!pBuf){
      return ENOMEMORY;
   }

   nRead = IFILE_Read(pIFile, pBuf, dwReqSize);

   if (nRead < (int)dwReqSize)
   {
      FREE(pBuf);
      return MM_ENOTENOUGHTDATA;
   }

   nErr = ISHELL_DetectType(ps, pBuf, &dwReqSize, NULL, &cpszMIME);

   if (!nErr){
      nErr = MGMediaInfo_MimeStrToType(cpszMIME, MimeBase, peMime);
   }

   if (pBuf)
      FREE(pBuf);

   return nErr;
}//MGMediaInfo_DetectSndVideoType


/*
 * ==========================================================================
 * FUNCTION     :  MGMediaInof_ValidateMime
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MGMediaInof_ValidateMime(MGMimeType eBaseMime, MGMimeType eItmeMime)
{
   boolean bRet = TRUE;

   if( MG_MIME_SNDBASE == eBaseMime)
   {
      bRet = MG_BETWEEN(eBaseMime, MG_MIME_SNDBASE, MG_MIME_SNDMAX) ||
            (eBaseMime == MG_MIME_VOICEREC) ||
            (eBaseMime == MG_MIME_CALLREC);
   }
   else if(MG_MIME_IMGBASE == eBaseMime)
   {
      bRet = MG_BETWEEN(eBaseMime, MG_MIME_IMGBASE, MG_MIME_IMGMAX);
   }
   else if(MG_MIME_VIDEOBASE == eBaseMime)
   {
      bRet = MG_BETWEEN(eBaseMime, MG_MIME_VIDEOBASE, MG_MIME_VIDEOMAX);
   }
   else if(MG_MIME_MISCELL == eBaseMime)
   {
      bRet = TRUE;
   }
   else{
      MG_FARF(STATE, ("Validate base mime"));
      bRet = (eBaseMime == eItmeMime);
   }

   return bRet;
}//MGMediaInof_ValidateMime

uint32 MGMediaInfo_GetID3V2(IFile* pIFile)
{
   //void* pBuffer;
   uint32 nRet;
   byte header[MGINFO_ID3V2_HEADERLEN];
   uint8 nCount;
   //byte *pVernier;
   uint32 nHeaderLen = 0;
   uint32 nByteConvert;

   if(!pIFile)
      return EFAILED;

   nRet = IFILE_Seek(pIFile, _SEEK_START, 0);
   if(SUCCESS != nRet)
      return nRet;

   nRet = IFILE_Read(pIFile, header, MGINFO_ID3V2_HEADERLEN);
   if(nRet != MGINFO_ID3V2_HEADERLEN)
      return EFAILED;

   if(FALSE == MEDIAINFO_IS_ID3V2_TAG(header))
      return SUCCESS;

   /*Get ID3v2 header size*/
   for(nCount = 0; nCount < ID3V2_HEADERLEN_BYTENUM; nCount++)
   {
      nByteConvert = header[MGINFO_ID3V2_HEADERLEN - 1 - nCount];
      nByteConvert <<= ID3V2_HEADERLEN_BYTEPOWER * nCount;
      nHeaderLen += nByteConvert;
   }

   /*ID3v2 flags*/
   if(header[5] & ID3V2_HEADERFLAG_FOOTERMASK)
   {
      MG_FARF(ADDR, ("ID3v2 have footer!"));
   }

   return SUCCESS;
}//MGMediaInfo_GetID3V2

uint32 MGMediaInfo_GetID3V1(IFile* pIFile, MGInfo_ID3V1* pTag)
{
   uint32 nRet;
   void* pBuffer;
   byte* pTemp;

   if(!pIFile || !pTag)
      return EBADPARM;

   nRet = IFILE_Seek(pIFile, _SEEK_END, - ID3V1_LEN);
   if(SUCCESS != nRet)
      return nRet;

   pBuffer = (void*)MALLOC(ID3V1_LEN);
   if(!pBuffer)
      return ENOMEMORY;

   nRet = IFILE_Read(pIFile, pBuffer, ID3V1_LEN);
   if(nRet != ID3V1_LEN)
      return EFAILED;

   nRet = MEMCMP(pBuffer, MEDIAINFO_ID3V1_TAG, STRLEN(MEDIAINFO_ID3V1_TAG));
   if(0 != nRet)
      return EFAILED;

   pTemp = pBuffer;
   pTemp += STRLEN(MEDIAINFO_ID3V1_TAG);

   MEMCPY(pTag->m_title, pTemp, sizeof(pTag->m_title));
   pTemp += sizeof(pTag->m_title);

   MEMCPY(pTag->m_artist, pTemp, sizeof(pTag->m_artist));
   pTemp += sizeof(pTag->m_artist);

   MEMCPY(pTag->m_album, pTemp, sizeof(pTag->m_album));
   pTemp += sizeof(pTag->m_album);

   MEMCPY(pTag->m_year, pTemp, sizeof(pTag->m_year));
   pTemp += sizeof(pTag->m_year);

   MEMCPY(pTag->m_comment, pTemp, sizeof(pTag->m_comment));
   pTemp += sizeof(pTag->m_comment) - sizeof(pTag->m_track);

// Check for ID3v1.1 -- Note that ID3v1 *does not* support "track zero" .
// Since a zeroed byte is what we would expect to indicate the end of a
// C-String, specifically the comment string, a value of zero must be assumed
// to be just that.

   if( 0 == *pTemp++ && 0 != *pTemp)
      pTag->m_track = (uint8)(*pTemp);
   else
      pTag->m_track = MGINFO_ID3V1_NONETRACK;

   pTag->m_genre = *++pTemp;

   FREEIF(pBuffer);
   return SUCCESS;
}//MGMediaInfo_GetID3V1

boolean MGMediaInfo_GetMP3Genre(byte nGenre, const AECHAR *pGenreStr)
{
   if(!pGenreStr)
      return FALSE;

   if( nGenre < ARRAYSIZE(ID3V1_Genres))
      return FALSE;

   pGenreStr = ID3V1_Genres[nGenre];

   return TRUE;
}

/* ===================== End of File ===================================== */
