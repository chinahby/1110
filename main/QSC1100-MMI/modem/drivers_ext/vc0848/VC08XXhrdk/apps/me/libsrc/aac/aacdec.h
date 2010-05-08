 
#ifndef __AACDEC_H__
#define __AACDEC_H__

#if defined(_WIN32) && !defined(_WIN32_WCE)
#
#elif defined(_WIN32) && defined(_WIN32_WCE) && defined(ARM)
#
#elif defined(_WIN32) && defined(WINCE_EMULATOR)
#
#elif defined (__arm) && defined (__ARMCC_VERSION)
#
#elif defined(_SYMBIAN) && defined(__WINS__)
#
#elif defined(__GNUC__) && defined(__arm__)
#
#elif defined(__GNUC__) && defined(__i386__)
#
#elif defined(__GNUC__) && defined(__amd64__)
#
#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__POWERPC__))
#
#elif defined(_OPENWAVE_SIMULATOR) || defined(_OPENWAVE_ARMULATOR)
#
#elif defined(_SOLARIS) && !defined(__GNUC__)
#
#else
#error No platform defined. See valid options in aacdec.h
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3)
 * max size of input buffer = 
 *    6144 bits =  768 bytes per SCE or CCE-I
 *   12288 bits = 1536 bytes per CPE
 *       0 bits =    0 bytes per CCE-D (uses bits from the SCE/CPE/CCE-I it is coupled to)
 */
#ifndef AAC_MAX_NCHANS				/* if max channels isn't set in makefile, */
#define AAC_MAX_NCHANS		2		/* set to default max number of channels  */
#endif
#define AAC_MAX_NSAMPS		1024
#define AAC_MAINBUF_SIZE	(768 * AAC_MAX_NCHANS)

#define AAC_NUM_PROFILES	3
#define AAC_PROFILE_MP		0
#define AAC_PROFILE_LC		1
#define AAC_PROFILE_SSR		2

/* define these to enable decoder features */
#if defined(AUDIO_CODEC_AAC_SBR)
#define AAC_ENABLE_SBR
#endif //  AUDIO_CODEC_AAC_SBR.

#define AAC_ENABLE_MPEG4

	
/* AAC file format */
enum {
	AAC_FF_Unknown = 0,		/* should be 0 on init */
	AAC_FF_ADTS = 1,
	AAC_FF_ADIF = 2,
	AAC_FF_RAW =  3		
};

enum {
	ERR_AAC_NONE                          =   0,
	ERR_AAC_INDATA_UNDERFLOW              =  -1,
	ERR_AAC_NULL_POINTER                  =  -2,
	ERR_AAC_INVALID_ADTS_HEADER           =  -3,
	ERR_AAC_INVALID_ADIF_HEADER           =  -4,
	ERR_AAC_INVALID_FRAME                 =  -5,
	ERR_AAC_MPEG4_UNSUPPORTED             =  -6,
	ERR_AAC_CHANNEL_MAP                   =  -7,
	ERR_AAC_SYNTAX_ELEMENT                =  -8,

	ERR_AAC_DEQUANT                       =  -9,
	ERR_AAC_STEREO_PROCESS                = -10,
	ERR_AAC_PNS                           = -11,
	ERR_AAC_SHORT_BLOCK_DEINT             = -12,
	ERR_AAC_TNS                           = -13,
	ERR_AAC_IMDCT                         = -14,
	ERR_AAC_NCHANS_TOO_HIGH               = -15,

	ERR_AAC_SBR_INIT                      = -16,
	ERR_AAC_SBR_BITSTREAM                 = -17,
	ERR_AAC_SBR_DATA                      = -18,
	ERR_AAC_SBR_PCM_FORMAT                = -19,
	ERR_AAC_SBR_NCHANS_TOO_HIGH           = -20,
	ERR_AAC_SBR_SINGLERATE_UNSUPPORTED    = -21,

	ERR_AAC_RAWBLOCK_PARAMS               = -22,

	ERR_OFFSET_TOO_LARGE                  = -23,
	ERR_COEF_NUM						  = -24,
	ERR_SECTION_DATA                      = -25,
	ERR_AUDIO_INFO_NOT_ENOUGH			  = -26,
	ERR_AUDIO_INFO_INVALID				  = -27,
		
		ERR_HUFF_DEC						  = -1000,
		
	ERR_UNKNOWN               = -9999
};

typedef struct _AACFrameInfo {
	int bitRate;
	int nChans;
	int sampRateCore;
	int sampRateOut;
	int bitsPerSample;
	int outputSamps;
	int profile;
	int tnsUsed;
	int pnsUsed;
	int format;
} AACFrameInfo;

typedef void *HAACDecoder;

/* public C API */
char* AACDecoderGetVersionInfo(void);
unsigned int AACDecoderGetSize(int bSBRSwitch);

//int AACInitDecoder(HAACDecoder hAACDecoder);
int  AACInitDecoder(HAACDecoder hAACDecoder, int bSBRSwitch);

//void AACFreeDecoder(HAACDecoder hAACDecoder);
int  AACDecode(HAACDecoder hAACDecoder, unsigned char **inbuf, int *bytesLeft, short *outbuf);

int  AACFindSyncWord(unsigned char *buf, int nBytes);
int  AACFlushCodec(HAACDecoder hAACDecoder);

void AACGetLastFrameInfo(HAACDecoder hAACDecoder, AACFrameInfo *aacFrameInfo);

int  AACSetRawBlockParams(HAACDecoder hAACDecoder, int copyLast, AACFrameInfo *aacFrameInfo);

#ifdef __cplusplus
}
#endif

#endif	//	__AACDEC_H__
