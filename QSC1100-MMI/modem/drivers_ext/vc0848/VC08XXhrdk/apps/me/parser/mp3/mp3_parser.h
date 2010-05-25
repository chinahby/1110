/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	mp3_parser.h
* User for : 	820
* Creat By :   Zhang Xuecheng()                                                                   
* CrateDate: 	2007/09/20
* Last modify : Zhang Xuecheng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Zhang Xuecheng
=======================================================================*/
#ifndef MP3_PARSER_H
#define MP3_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "..\..\me_comhead.h"
#include "..\..\libsrc\mp3\mpegaudiodecoder.h"
#define MP3_DECODE_MULTI_FRAME 1
#define 	SYNC_TIMES						1024*8
#define 	MAX_FRAME_BUFFER_SIZE		1024*8
#define	MAX_SYNCHS					30

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

//: Add by antonliu, 2008/06/03
#define MP3_FRAME_SYNC 		21
#define MP3_AUDIO_VERSION 		19
#define MP3_LAYER				17
#define MP3_PROTECT_BIT		16
#define MP3_BITRATE_IDX		12
#define MP3_SAMPLERATE_IDX	10
#define MP3_PADDING_BIT		9
#define MP3_PRIVATE_BIT			8
#define MP3_CHANNEL_MODE		6
#define MP3_MODE_EXT			4
#define MP3_COPYRIGHT			3
#define MP3_ORIGINAL			2
#define MP3_EMPHASIS			0

#define VERSION_MPEG_2_5		0
#define VERSION_MPEG_2			2
#define VERSION_MPEG_1			3

#define MPEG_LAYER_3			1
#define MPEG_LAYER_2			2
#define MPEG_LAYER_1			3

#define CHN_MODE_S				0
#define CHN_MODE_JS			1
#define CHN_MODE_DUAL			2
#define CHN_MODE_MONO			3


typedef struct
{
    int stereo;
    int jsbound;
    int single;
    int lsf;
    int mpeg25;
	int lay;
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
    int framesize; /* computed framesize */
} MPEGAudioFrameInfo;

#define MAX_BUF_LENGTH						4096

#if defined (__arm) && defined (__ARMCC_VERSION)
#define ARMULATE_MUL_FACT	1
#define MAX_FRAMES	-1 
#else
#define ARMULATE_MUL_FACT	1
#define MAX_FRAMES	-1
#endif
#define READBUF_SIZE	4096

#define uint32 int

/*original value*/
#define LAYER_1				3
#define LAYER_2				2
#define LAYER_3				1

/*mapped val*/
#define LAYER_1_MAP			1
#define LAYER_2_MAP			2
#define LAYER_3_MAP			3

#define BYTES_PER_SLOT_L1   4
#define BYTES_PER_SLOT_L23  1


#define LAYERS				3					/* 0:res 1:l3 2:l2 3:l1              */
#define MPEG_VERSION_IDS	3					/* original:		0:mpeg2.5  1:res   2:mpeg2  3:mpeg2
												  after mapping:	0:mpeg1    1:mpeg2  2:mpeg2.5  */
#define MAX_SCFBD			4					/* layer 3 : max scalefactor bands per channel */
#define NGRANS_MPEG1		2
#define NGRANS_MPEG2		1

#define BITRATES			16
#define SAMPL_FREQS			3


/* 11-bit syncword if MPEG 2.5 extensions are enabled */
#define	SYNCWORDH			0xFF
#define	SYNCWORDL			0xE0

#define FRAME_HEADER_LEN		4					/*byte unit*/
#define FRAME_HEADER_LEN_CRC	6					/*byte unit*/

#define HEADER_MASK			0xFFFE0C00	
		
#define SYNCH_MASK			0xFFE00000				/* 11 */
#define MPEGID_MASK			0x00180000				/*  2 */
#define LAYER_MASK			0x00060000				/*  2 */
#define PROTECT_BIT_MASK	0x00010000				/*  1 */
#define BITRATE_IDX_MASK	0x0000F000				/*  4 */	
#define SAMPLERATE_IDX_MASK	0x00000C00				/*  2 */
#define PADDING_MASK		0x00000200				/*  1 */
#define PRIVATE_BIT_MASK	0x00000100				/*  1 */
#define STEREO_MODE_MASK	0x000000C0				/*  2 */
#define MOD_EXT_MASK		0x00000030				/*  2 */
#define COPY_FLAG_MASK		0x00000008				/*  1 */
#define ORI_FLAG_MASK		0x00000004				/*  1 */
#define EMPHASIS_MASK		0x00000003				/*  2 */	


#define EMPHASIS_RES_VAL	0x2
#define MPEGID_RES_VAL		0x1
#define LAYER_RES_VAL		0x0
#define BITRATE_IDX_RES_VAL	0xF
#define SAMPLERATE_IDX_RES_VAL	0x3


#define MAX_SYNCHS			30					/*max #head stored in scratch buf*/
#define MAX_HEAD_SI_BITS	304					/* max #bits in header,crc and SI */
#define HEADER_BITS			32


#define POLY_BANDS			32
#define L1_BLOCK_SIZE		12
#define MDCT_BANDS			18
#define	HAN_SIZE			512

#define QMF_SYN_WINDOW_LEN	512
#define MAX_BLOCK_SIZE		18					/*36 indeeded, to save mem*/
#define L3_BLOCK_SIZE		18

#define DCT_SIZE			32

#define MAX_WORKBUF_SIZE	2700//(3*1024)			/*max #bytes of all layer workbuf*/
#define SYNCBUF_SIZE		200

#define MAX_REORDER_SAMPS	((192-126)*3)		/* largest critical band for short blocks (see sfBandTable) */
#define	IMDCT_SCALE			2					/* additional scaling (by sqrt(2)) for fast IMDCT36 */


/* map to 0,1,2 to make table indexing easier */
typedef enum {
	MPEG1 =  0,
	MPEG2 =  1,
	MPEG25 = 2
} MPEGVersion;

/* map these to the corresponding 2-bit values in the frame header */
typedef enum {
	Stereo = 0x00,	/* two independent channels, but L and R frames might have different # of bits */
	Joint  = 0x01,	/* coupled channels - layer III: mix of M-S and intensity, Layers I/II: intensity and direct coding only */
	Dual   = 0x02,	/* two independent channels, L and R always have exactly 1/2 the total bitrate */
	Mono   = 0x03		/* one channel */
} StereoMode;

/* Enumeration of error codes */
typedef  enum{			
	mp3dec_ERR_OK = 0,		
	mp3dec_ERR_NULL_POINTER,					/* NULL pointer used */			
	mp3dec_ERR_FRAME_NOT_COMPLETE,				/*no one complete frame of mp3 stream in stream buff */				
	mp3dec_ERR_INDATA_UNDERFLOW,				/*only useful in  MP3StartDecode() to indicate that not enough syncwords are found, but at least one is found*/			
	mp3dec_ERR_MAINDATA_UNDERFLOW,				/*no enough main data in main data buf (Layer3 only)*/				
	mp3dec_ERR_NO_SYNC,							/*no syncword in remaining streams in stream buff*/		
	mp3dec_ERR_INVALID_SYNC,					/*invalid syncword*/				
	mp3dec_ERR_CRC,								/*crc chk err*/		
	mp3dec_ERR_INVALID_L1_ALLOCATION_BITS,		/*invalid allocation bits number(layer1 only)*/		
	mp3dec_ERR_INVALID_AUDIO_DATA,				/*invalid audio stream*/			
	mp3dec_ERR_HUFFMAN_DECODING,				/*err occur during huffman decoding */								
	mp3dec_ERR_INVALID_HUFFMAN_TABLE_NO,		/*invalid huffman table no*/		
	mp3dec_ERR_INVALID_SCALEFACTOR_IDX,			/*invalid scf index(layer 1/2 only)*/		
	mp3dec_ERR_L3SCF_DECODING,					/*err occur during scf decoding (layer3 only)*/		
	mp3dec_ERR_SIDE_INFO_BAD,					/*invalid sideinfo(layer3 only)*/		
	mp3dec_ERR_TOO_MUCH_SAMP,					/* samp number > MAX_NSAMP(layer3 only) */		
	mp3dec_ERR_DEQUANT,							/*err occur in dequantization module(layer3 only)*/		
	mp3dec_ERR_IMDCT,							/*err occur in imdct module(layer3 only)*/				
	mp3dec_ERR_UNKNOWN = 9999		
} mp3dec_Err;

/*sync info struct*/
typedef struct _Synch
{
    unsigned char active;
	unsigned char paired_flag;
	unsigned char paired_last_sync_idx;
	unsigned char bitrate_idx_zero_flag;
	unsigned char padding;
    char count;

	int next_synch_offset;
    int next_header_distance;
//  int last_header_distance;
    unsigned int header;
	int offset;
	
    
//  int bitrate;
}Synch;

typedef struct _SynchInfo
{
    int max_synch_index;
    int offset_current;
	Synch synchs[MAX_SYNCHS];
	
}SynchInfo;

typedef struct _FrameHeader {
    MPEGVersion ver;	/* version ID */
	unsigned short crc_check;		/* frame CRC accumulator */
	unsigned short crc_target;		/* target CRC checksum */
    int layer;			/* layer index (1, 2, or 3) */
    int crc;			/* CRC flag: 0 = disabled, 1 = enabled */
    int brIdx;			/* bitrate index (0 - 15) */
    int srIdx;			/* sample rate index (0 - 2) */
    int paddingBit;		/* padding flag: 0 = no padding, 1 = single pad slot */
//  int privateBit;		/* unused */
    StereoMode sMode;	/* mono/stereo mode */
    int modeExt;		/* used to decipher joint stereo mode */
    int copyFlag;		/* copyright flag: 0 = no, 1 = yes */
    int origFlag;		/* original flag: 0 = copy, 1 = original */
    int emphasis;		/* deemphasis mode */
	int fhLen;			/*frame header length, including crc bytes, bytes unit*/
} FrameHeader;


typedef struct _MP3DecInfo {


	/* general dynamic data, active at all times */
    int state;
    unsigned int header_word_bakup;
	int header_set_flag;
	int last_frame_ok_flag;

	/* special info for "free" bitrate files */
	int freeBitrateFlag;
	int freeBitrateSlots;
	
	/* user-accessible info */
	MPEGVersion version;
	int bitrate;
	int nChans;
	int samprate;
	int nGrans;				/* granules per frame */
	int nGranSamps;			/* samples per granule */
	int nSampsPerCh;
	int layer;
	

	int next_header_distance;   /* distance to next header in bytes */
	int frame_length;

	FrameHeader S_FrameHeader;

	
	/*sync buf*/
	int SyncBuf[SYNCBUF_SIZE];		/*special buf may be redundant,but ease and safe*/


} MP3DecInfo;

typedef struct
{
	void*		stream;
	ME_U32 		uloffset;				/*stream offset */
	
	ME_S32 		(*read_func)(void *file, char* buffer, int readbytes);
	ME_S32 		(*write_func)(void *file, char* buffer, int writebytes);
	ME_S32 		(*seek_func)(void *file, int i_offset, int mode);
	ME_S32 		(*tell_func)(void *file);
	
	ME_S32		(*read_block_func)(void *file, char* buffer, ME_S32 readbytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*write_block_func)(void *file, char* buffer, ME_S32 writebytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*get_offset_sector_func)(void *file, ME_U32 *sectorsize);
	
	
	ME_U32		first_frame_offset;	/* the offset of first audio data,equal to file length - id3v2's length*/
	ME_U32		id3_size;			/* id3v1's length + id3v2's length*/
	ME_U32		file_length;			/* the file's length */
	
	ME_U32		a_vbr;				/* 0:cbr;1:vbr or abr */
	ME_U32		a_bits;				/* bit length */
	ME_U32		a_chn;				/* 1: mono, 2: stereo */
	ME_U32		a_smplrate;			/* sample rate */
	ME_U32		a_bitrate;			/* bitrate */
	ME_U32		a_smplcount;		/* sample number of 1 frame */
	ME_U32		a_frmsize;			/* frame size(if CBR frame size, else = 0) */
	ME_U32		a_duration;			/* frame duration(if changeless frame rate or first frame duration) */
	ME_U32		a_totaltime;			/* audio total play time (ms)*/
	ME_U32		a_totalframe;		/* audio total frame*/
	ME_S08		a_codec[4];			/* string "mp3" */
	ME_U32		a_layer;				/* mp3 layer  */

	ME_U32		is_mp3;
	MP3DecInfo*  mp3DecInfo;		


} mp3_infor_t;

ME_U32 mp3_read_audio_frame_ver3(mp3_infor_t *file, audio_stream_infor* audio_buffer, ME_U32 reg, ME_U32 bit);
ME_U32 mp3_read_audio_frame_ver2(mp3_infor_t *file, audio_stream_infor* audio_buffer);
ME_U32 mp3_read_audio_frame_ver3_post(mp3_infor_t *file, audio_stream_infor *audio_buffer);
ME_U32 mp3_readaudio(mp3_infor_t *file,char *audio_buffer, unsigned int *readbytes,unsigned int bufferlength);
ME_U32 mp3_read_info(mp3_infor_t *file);
mp3_infor_t* mp3_init(void);
ME_U32 mp3_close(mp3_infor_t *file);
ME_U32 mp3_check_sig(mp3_infor_t *file);
ME_U32 mp3_getmaxframesize(mp3_infor_t *file,unsigned int *framesize);
ME_U32 mp3_getaudioduration(mp3_infor_t *fileinfor, unsigned int frame, unsigned int * duration);
ME_U32 mp3_get_framecnt(mp3_infor_t *fileinfor,unsigned int *framecnt);
ME_U32 mp3_get_framelength(mp3_infor_t *fileinfor,unsigned int frameindex,unsigned int *framelen);
ME_U32 mp3_analyse_stream(mp3_infor_t* file , ME_U32 segment , ME_U32 frames, ME_U32 frm_len);
unsigned int compute_frame_size(	int mpegid , 
									int layer,
									int bitrate, 
									int sampfreq, 
									int padding 
								);
//extern int SearchSyncword(unsigned char *buf,int *buflen);
extern ME_U32 search_syncword(unsigned char *buf,int *buflen);

#endif

