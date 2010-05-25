/******************************************************************************
               (C) copyright (2006)  Vimicro  All Rights Reserved
 ******************************************************************************/

/******************************************************************************
 File:	aacparselib.h
 Date:	2006-3-6   16:53
 ------------------------------------------------------------------------------
 Description:
	
 ******************************************************************************/

/******************************************************************************
                      C H A N G E   R E C O R D

 #   Date		     Author			Description
 --  ----------	 ------------------  --------------------------------------
 01  2006-3-6	Xiaocheng Wang		Created this file
 ******************************************************************************/
#ifndef __AACPARSELIB_H__
#define __AACPARSELIB_H__

#if defined(__cplusplus)
extern "C"{
#endif
	
/******************************************************************************
							  Include files
 ******************************************************************************/
#include"aacdec.h"
/******************************************************************************
							  Macro definitions
 ******************************************************************************/
#define AAC_MAX_NCHANS		2		// set to default max number of channels  
#define AAC_MAX_NSAMPS		1024
#define AAC_MAINBUF_SIZE	(768 * AAC_MAX_NCHANS)

#define ADIF_COPYID_SIZE        9

#define AAC_PROFILE_LC		1

#define MAX_NUM_PCE_ADIF	16

// for fixed part. minus 8 bits of syncword, so only 20 bit belongs to fixed part
#define AAC_ADTS_FIXEDHEADER_COPY_SIZE		3

/******************************************************************************
							  Type definitions
 ******************************************************************************/
enum {
	ERR_AACPARSER_NONE					=   0,
	ERR_AACPARSER_INDATA_UNDERFLOW		=  -1,
	ERR_AACPARSER_NULL_POINTER			=  -2,
	ERR_AACPARSER_INVALID_ADTS_HEADER	=  -3,
	ERR_AACPARSER_INVALID_ADIF_HEADER	=  -4,
	ERR_AACPARSER_INVALID_FRAME			=  -5,
	ERR_AACPARSER_UNKNOWN_FORMAT		=  -6,
	ERR_AACPARSER_CHANNEL_MAP			=  -7,
	ERR_AACPARSER_SYNTAX_ELEMENT		=  -8,
	
	ERR_AACPARSER_NCHANS_TOO_HIGH		=  -9,
	
	ERR_AACPARSER_UNKNOWN				= -9999
};

//deleted by xul, for duplicated by aacdec.h
// AAC file format type  
/*
enum {
	AAC_FF_Unknown	= 0,		// should be 0 on init 

	AAC_FF_ADTS		= 1,
	AAC_FF_ADIF		= 2,
	AAC_FF_RAW		= 3
};
*/
typedef struct _ADTSHeader 
{
    //fixed 
    unsigned char id;                             /* MPEG bit - should be 1 */
    unsigned char layer;                          /* MPEG layer - should be 0 */
    unsigned char protectBit;                     /* 0 = CRC word follows, 1 = no CRC word */
    unsigned char profile;                        /* 0 = main, 1 = LC, 2 = SSR, 3 = reserved */
    unsigned char sampRateIdx;                    /* sample rate index range = [0, 11] */
    unsigned char privateBit;                     /* ignore */
    unsigned char channelConfig;                  /* 0 = implicit, >0 = use default table */
    unsigned char origCopy;                       /* 0 = copy, 1 = original */
    unsigned char home;                           /* ignore */

    //variable 
    unsigned char copyBit;                        /* 1 bit of the 72-bit copyright ID (transmitted as 1 bit per frame) */
    unsigned char copyStart;                      /* 1 = this bit starts the 72-bit ID, 0 = it does not */
    int           frameLength;                    /* length of frame */
    int           bufferFull;                     /* number of 32-bit words left in enc buffer, 0x7FF = VBR */
    unsigned char numRawDataBlocks;               /* number of raw data blocks in frame */

    //CRC 
    int           crcCheckWord;                   /* 16-bit CRC check word (present if protectBit == 0) */
} ADTSHeader;

typedef struct _ADIFHeader 
{
    unsigned char copyBit;                        /* 0 = no copyright ID, 1 = 72-bit copyright ID follows immediately */
    unsigned char origCopy;                       /* 0 = copy, 1 = original */
    unsigned char home;                           /* ignore */
    unsigned char bsType;                         /* bitstream type: 0 = CBR, 1 = VBR */
    int           bitRate;                        /* bitRate: CBR = bits/sec, VBR = peak bits/frame, 0 = unknown */
    unsigned char numPCE;                         /* number of program config elements (max = 16) */
    int           bufferFull;                     /* bits left in bit reservoir */
    unsigned char copyID[ADIF_COPYID_SIZE];       /* optional 72-bit copyright ID */
} ADIFHeader;

typedef struct _AACInfo 
{
	ADIFHeader	fhADIF;
	ADTSHeader	fhADTS;

	// for fixed part, only 28 bit belongs to fixed part
	unsigned char ADTSheaderBackup[AAC_ADTS_FIXEDHEADER_COPY_SIZE];

//	ProgConfigElement     pce;
	void*	pce;
	
	// user-accessible info 
	int bitRate;
	int nChans;
	int sampRate;		// output sample rate
	int sampRateCore;	// core sample rate (aac sample rate), equals to sampRate
	int profile;
	int format;
//	int sbrEnabled;		// means nothing because this information is not parsed
//	int tnsUsed;		// means nothing because this information is not parsed
//	int pnsUsed;		// means nothing because this information is not parsed
	
	int outputSamps;

	unsigned int frameCount;		// Total block count
    unsigned int nDataCount;		// Total Data count in bytes
	
    //state information which is the same throughout whole frame 
    int useImpChanMap;
    int sampRateIdx;
} AACInfo;

typedef AACInfo *HAACParser;
/******************************************************************************
							  Constant definitions
 ******************************************************************************/

/******************************************************************************
							  Function prototypes
 ******************************************************************************/
// public C API 
HAACParser AACParserLib_Create(void);
void AACParserLib_Release(HAACParser hAACParser);

int  AACParserLib_Open(HAACParser pAACInfo, unsigned char **inbuf, int *bytesLeft);
int  AACParserLib_ADTSParsing(HAACParser hAACParser, unsigned char **inbuf, int *bytesLeft);


//---------------define for message debug for memory lost 2007/04/20---------------------------

void *aac_parser_malloc( unsigned int size);
void aac_parser_free( void *memblock );

#define 	AAC_PARSER_MALLOC_DEBUG			0

#if	AAC_PARSER_MALLOC_DEBUG
#define 	AAC_PARSER_MALLOC_MSG(str)		{me_printf("AAC_PARSER_MALLOC: ");me_printf str;}
#define 	AAC_PARSER_FREE_MSG(str)			{me_printf("AAC_PARSER_FREE:      ");me_printf str;}
#define 	AAC_PARSER_TRACE_MSG()			{me_printf("-*-%s,line=%d\n",__FILE__,__LINE__);}
#else
#define 	AAC_PARSER_MALLOC_MSG(str)	
#define 	AAC_PARSER_FREE_MSG(str)	
#define 	AAC_PARSER_TRACE_MSG()			

#endif


#if defined(__cplusplus)
}
#endif

#endif		// __AACPARSELIB_H__
