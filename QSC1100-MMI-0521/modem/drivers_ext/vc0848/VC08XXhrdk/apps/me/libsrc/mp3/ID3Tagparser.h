# ifndef MP3PARSER_H
# define MP3PARSER_H

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

#define MP3_ID3V2_HEADERSIZE 10 //ID3V2 tag header length (Byte)
#define         MP3_GENRE_NUM        148

//frame id of ID3V2.2.0
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

//Mp3 ID3V1 version 0
typedef struct{
	//UINT8 bTag[3];                                 /* Tag flag               */
	UINT8 bTitle[30];                              /* Mp3 Title              */ 
	UINT8 bArtist[30];                             /* Mp3 artist             */
	UINT8 bAlbum[30];                              /* Mp3 album              */
	UINT8 bYear[5];                                /* Mp3 Year               */
	UINT8 bComment[30];                            /* Mp3 Comment            */
	UINT8 bGenre;                                  /* Mp3 Genre              */
}MP3ID3V1_0;

//Mp3 ID3V1 version 1
typedef struct{
	//UINT8 bTag[3];                                 /* Tag flag               */
	UINT8 bTitle[30];                              /* Mp3 Title              */ 
	UINT8 bArtist[30];                             /* Mp3 artist             */
	UINT8 bAlbum[30];                              /* Mp3 album              */
	UINT8 bYear[5];                                /* Mp3 Year               */
	UINT8 bComment[28];                            /* Mp3 Comment            */
    UINT8 sZero;                                   /* Mp3 Zero               */
    UINT8 sTrack;                                  /* Mp3 Track              */
	UINT8 bGenre;                                  /* Mp3 Genre              */
}MP3ID3V1_1;

//Mp3 ID3V2 Info
typedef struct{
	//UINT8 bTag[3];                                 /* Tag flag               */
	UINT8 bTitle[30];                              /* Mp3 Title              */ 
	UINT8 bArtist[30];                             /* Mp3 artist             */
	UINT8 bAlbum[30];                              /* Mp3 album              */
	UINT8 bYear[5];                                /* Mp3 Year               */
	//UINT8 bComment[30];                          /* Mp3 Comment            */
	UINT8 bComment[29];                            /* Mp3 Comment            */
    UINT8 sTrack;                                  /* Mp3 Track              */
	UINT8 bGenre;                                  /* Mp3 Genre              */
}MP3ID3V2;

//MP3 ID3 tag
typedef struct{
	UINT8 bVersion;                                /* The ID3 version        */
	PUINT8 bMp3Genre;                              /* The Mp3 Genre          */
    union {
        MP3ID3V1_0 V1_0;                           /* The Mp3 ID3V1_0 version*/
        MP3ID3V1_1 V1_1;                           /* The Mp3 ID3V1_1 version*/
		MP3ID3V2   V2;                             /* The Mp3 ID3V2 version  */
    }ID3;
} MP3_ID3TAG;

//MP3 ID3V2 header
typedef struct
{
	UINT8  bMajorVerison;							/*Major version  = 3 	 */
	UINT8  bRevisionVersion;    					/*Revision version		 */
	UINT8  bFlag;    								/*defined 3 bit			 */
	UINT32 dSize;    					    		/*the ID3 FRAME SIZE 	 */
}MP3_ID3V2HEADER;

//MP3 ID3V2 frame header
typedef struct
{
	UINT8  bFrameID[4];								/*frame id				 */
	UINT32 dSize;									/*frame size ,not less 1 */ 
	UINT8  bFlags[2];								/*save flag,defined 6 bit*/
}MP3_ID3V2FRAMEHEADER;


int CheckID3V1Tag(MP3_ID3TAG *pTag, unsigned char *buf, long buflen);
int CheckID3V2Tag(unsigned char *buf, long buflen);

#endif

