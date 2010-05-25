/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	mp3_id3.h
* User for : 	820
* Creat By :   Zhang Xuecheng()                                                                   
* CrateDate: 	2007/08/21
* Last modify : Zhang Xuecheng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Zhang Xuecheng
=======================================================================*/

#ifndef _MP3_ID3_H_
#define _MP3_ID3_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "me_comhead.h"
#include "me_typedef.h"
#include "me_object.h"
#include "me_rtnflag.h"
#include "me_keymacro.h"


#ifndef UINT8
#define UINT8  unsigned char
#endif
#ifndef PUINT8
#define PUINT8 unsigned char*
#endif
#ifndef UINT32
#define UINT32 unsigned int
#endif
#ifndef SINT32 
#define SINT32 int
#endif

typedef unsigned char id3_byte_t;

#define 	MP3_ID3_MALLOC_DEBUG			0
#define MP3_ID3V2_HEADERSIZE 10 //ID3V2 tag header length (Byte)
#define         MP3_GENRE_NUM        148

#define MP3_ID3V220_FRAMEHEADERSIZE 6//ID3V2.2.0 frame header size(Byte) 
#define ID3V220_FRAME_TITLE	"TT2"
#define ID3V220_FRAME_ARTIST	"TP1"
#define ID3V220_FRAME_ALBUM	"TAL"
#define ID3V220_FRAME_YEAR		"TYE"
#define ID3V220_FRAME_COMMENT	"COM"
#define ID3V220_FRAME_TRACK	"TRK"
#define ID3V220_FRAME_GENRE	"TCO"

//frame id of ID3V2.3.0  
#define MP3_ID3V2_FRAMEHEADERSIZE  10 //ID3V2 frame header size(Byte)
#define ID3_FRAME_TITLE	"TIT2"
#define ID3_FRAME_ARTIST	"TPE1"
#define ID3_FRAME_ALBUM	"TALB"
#define ID3_FRAME_YEAR		"TYER"
#define ID3_FRAME_COMMENT	"COMM"
#define ID3_FRAME_TRACK	"TRCK"
#define ID3_FRAME_GENRE	"TCON"

//extended header of ID3V2.3.0 and ID3V2.4.0
#define Extended_HEADER_MASK 0x40

//FOOTER header of ID3V2.4.0
#define FOOTER_HEADER_MASK  0x10
#define FOOTER_HEADER_SIZE  10

#if	MP3_ID3_MALLOC_DEBUG
#define 	MP3_ID3_MALLOC_MSG(str)		{me_printf("MP3_ID3_MALLOC: ");me_printf str;}
#define 	MP3_ID3_FREE_MSG(str)			{me_printf("MP3_ID3_FREE:      ");me_printf str;}
#define 	MP3_ID3_TRACE_MSG()			{me_printf("-*-%s,line=%d\n",__FILE__,__LINE__);}
#else
#define 	MP3_ID3_MALLOC_MSG(str)	
#define 	MP3_ID3_FREE_MSG(str)	
#define 	MP3_ID3_TRACE_MSG()			
#endif

static const unsigned char g_Genre[MP3_GENRE_NUM][23] = {
    "Blues",
    "Classic Rock",
    "Country",
    "Dance",
    "Disco",
    "Funk",
    "Grunge",
    "Hip-Hop",
    "Jazz",
    "Metal",
    "New Age",
    "Oldies",
    "Other",
    "Pop",
    "R&B",
    "Rap",
    "Reggae",
    "Rock",
    "Techno",
    "Industrial",
    "Alternative",
    "Ska",
    "Death Metal",
    "Pranks",
    "Soundtrack",
    "Euro-Techno",
    "Ambient",
    "Trip-Hop",
    "Vocal",
    "Jazz+Funk",
    "Fusion",
    "Trance",
    "Classical",
    "Instrumental",
    "Acid",
    "House",
    "Game",
    "Sound Clip",
    "Gospel",
    "Noise",
    "Alternative Rock",
    "Bass",
    "Soul",
    "Punk",
    "Space",
    "Meditative",
    "Instrumental Pop",
    "Instrumental Rock",
    "Ethnic",
    "Gothic",
    "Darkwave",
    "Techno-Industrial",
    "Electronic",
    "Pop-Folk",
    "Eurodance",
    "Dream",
    "Southern Rock",
    "Comedy",
    "Cult",
    "Gangsta",
    "Top 40",
    "Christian Rap",
    "Pop/Funk",
    "Jungle",
    "Native American",
    "Cabaret",
    "New Wave",
    "Psychadelic",
    "Rave",
    "Showtunes",
    "Trailer",
    "Lo-Fi",
    "Tribal",
    "Acid Punk",
    "Acid Jazz",
    "Polka",
    "Retro",
    "Musical",
    "Rock & Roll",
    "Hard Rock",
    "Folk",
    "Folk-Rock",
    "National Folk",
    "Swing",
    "Fast Fusion",
    "Bebob",
    "Latin",
    "Revival",
    "Celtic",
    "Bluegrass",
    "Avantgarde",
    "Gothic Rock",
    "Progressive Rock",
    "Psychedelic Rock",
    "Symphonic Rock",
    "Slow Rock",
    "Big Band",
    "Chorus",
    "Easy Listening",
    "Acoustic",
    "Humour",
    "Speech",
    "Chanson",
    "Opera",
    "Chamber Music",
    "Sonata",
    "Symphony",
    "Booty Bass",
    "Primus",
    "Porn Groove",
    "Satire",
    "Slow Jam",
    "Club",
    "Tango",
    "Samba",
    "Folklore",
    "Ballad",
    "Power Ballad",
    "Rhythmic Soul",
    "Freestyle",
    "Duet",
    "Punk Rock",
    "Drum Solo",
    "Acapella",
    "Euro-House",
    "Dance Hall",
    "Goa", 
    "Drum & Bass", 
    "Club-House", 
    "Hardcore", 
    "Terror", 
    "Indie", 
    "BritPop", 
    "Negerpunk", 
    "Polsk Punk", 
    "Beat", 
    "Christian Gangsta Rap", 
    "Heavy Metal", 
    "Black Metal", 
    "Crossover", 
    "Contemporary Christian", 
    "Christian Rock",
    "Merengue", 
    "Salsa", 
    "Thrash Metal", 
    "Anime", 
    "Jpop", 
    "Synthpop" 
};

static const unsigned char g_GenreUnkonwn[8] = "unknown";


//Mp3 ID3V1 version 0
typedef struct{
	//unsigned char bTag[3];                                 /* Tag flag               */
	unsigned char bTitle[30];                              /* Mp3 Title              */ 
	unsigned char bArtist[30];                             /* Mp3 artist             */
	unsigned char bAlbum[30];                              /* Mp3 album              */
	unsigned char bYear[5];                                /* Mp3 Year               */
	unsigned char bComment[30];                            /* Mp3 Comment            */
	unsigned char bGenre;                                  /* Mp3 Genre              */
}MP3ID3V1_0;

//Mp3 ID3V1 version 1
typedef struct{
	//unsigned char bTag[3];                                 /* Tag flag               */
	unsigned char bTitle[30];                              /* Mp3 Title              */ 
	unsigned char bArtist[30];                             /* Mp3 artist             */
	unsigned char bAlbum[30];                              /* Mp3 album              */
	unsigned char bYear[5];                                /* Mp3 Year               */
	unsigned char bComment[28];                            /* Mp3 Comment            */
    unsigned char sZero;                                   /* Mp3 Zero               */
    unsigned char sTrack;                                  /* Mp3 Track              */
	unsigned char bGenre;                                  /* Mp3 Genre              */
}MP3ID3V1_1;

//Mp3 ID3V2 Info
typedef struct{
	//unsigned char bTag[3];                                 /* Tag flag               */
	unsigned char bTitle[30];                              /* Mp3 Title              */ 
	unsigned char bArtist[30];                             /* Mp3 artist             */
	unsigned char bAlbum[30];                              /* Mp3 album              */
	unsigned char bYear[5];                                /* Mp3 Year               */
	//unsigned char bComment[30];                          /* Mp3 Comment            */
	unsigned char bComment[29];                            /* Mp3 Comment            */
    unsigned char sTrack;                                  /* Mp3 Track              */
	unsigned char bGenre;                                  /* Mp3 Genre              */
}MP3ID3V2;

//MP3 ID3 tag
typedef struct{

	unsigned char bVersion;                                /* The ID3 version        */
	unsigned char * bMp3Genre;                              /* The Mp3 Genre          */

	union {
        	MP3ID3V1_0 V1_0;                           /* The Mp3 ID3V1_0 version*/
        	MP3ID3V1_1 V1_1;                           /* The Mp3 ID3V1_1 version*/
		MP3ID3V2   V2;                             /* The Mp3 ID3V2 version  */
    	}ID3;

	void* stream;
	unsigned long offset;
} mp3_id3_infor_t;

//MP3 ID3V2 header
typedef struct
{
	unsigned char  bMajorVerison;							/*Major version  = 3 	 */
	unsigned char  bRevisionVersion;    					/*Revision version		 */
	unsigned char  bFlag;    								/*defined 3 bit			 */
	unsigned int dSize;    					    		/*the ID3 FRAME SIZE 	 */
}MP3_ID3V2HEADER;

//MP3 ID3V2 frame header
typedef struct
{
	unsigned char  bFrameID[4];								/*frame id				 */
	unsigned int dSize;									/*frame size ,not less 1 */ 
	unsigned char  bFlags[2];								/*save flag,defined 6 bit*/
}MP3_ID3V2FRAMEHEADER;


//////////////////////////////////////
int CheckID3V1Tag(mp3_id3_infor_t *pTag, unsigned char *buf, long buflen);
int CheckID3V2Tag(mp3_id3_infor_t *file, fs_callbacks *fs_filefunc);

ME_U32 mp3_id3_init(void *mediafile, fs_callbacks *fs_filefunc,void **id3infor);
ME_U32 mp3_id3_close(void *id3infor);
ME_U32 mp3_get_id3_info(mp3_id3_infor_t *file, fs_callbacks *fs_filefunc);
unsigned long mp3_id3v2_get_header_length(unsigned char **ptr, unsigned int bytes);
ME_U32 mp3_id3_get_property(void *id3infor, me_property*property);
int parser_ID3V220(mp3_id3_infor_t *file, fs_callbacks *fs_filefunc,unsigned char* buf,int sdSize);
unsigned long id3_parse_uint(unsigned char **ptr, unsigned int bytes);
ME_U32 mp3_read_buffer(void* buffer, int count,mp3_id3_infor_t *file,fs_callbacks *fs_filefunc);
unsigned long id3_parse_syncsafe(id3_byte_t **ptr, unsigned int bytes);
ME_U32 id3_mp3_register(void);

#endif
