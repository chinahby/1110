/******************************************************************************
               (C) copyright (2006)  Vimicro  All Rights Reserved
 ******************************************************************************/

/******************************************************************************
 File:	aacparselib.c
 Date:	2006-3-6   16:58
 ------------------------------------------------------------------------------
 Description:
	
 ******************************************************************************/

/******************************************************************************
                      C H A N G E   R E C O R D

 #   Date		     Author			Description
 --  ----------	 ------------------  --------------------------------------
 01  2006-3-6	Xiaocheng Wang		Created this file
 ******************************************************************************/

/******************************************************************************
							  Include files
 ******************************************************************************/
#include <String.h>
#include "AACParselib.h"
#include "AACparser.h"
#include "bitstream.h" 
//#include "me_utils.h"
/******************************************************************************
							  Macro definitions
 ******************************************************************************/
/******************************************************************************
							  Type definitions
 ******************************************************************************/
/******************************************************************************
							  Variable definitions
 ******************************************************************************/
 
//: Add by antonliu, 2008/0718
#define SYNCWORDH 	0xff
#define SYNCWORDL	0xf0

int  AACFindSyncWord(unsigned char *buf, int nBytes)
{
	int i;
	
	/* find byte-aligned syncword - need 12 (MPEG 1,2) or 11 (MPEG 2.5) matching bits */
	for (i = 0; i <= nBytes - 4; i++) {
		if ( (buf[i] & SYNCWORDH) == SYNCWORDH && (buf[i+1] & SYNCWORDL) == SYNCWORDL )
			return i;
	}
	
	return -1;
}



void *aac_parser_malloc( unsigned int size)
{
	void* memblock;
	memblock = (void *)me_malloc(size);
//	AAC_PARSER_MALLOC_MSG(("address=0x%x,size=%d\n", (unsigned int)memblock,size));
	return memblock;
}

void aac_parser_free( void *memblock )
{
//	AAC_PARSER_FREE_MSG(("address=0x%x\n", (unsigned int)memblock));
	me_free((void *)memblock );
}

/******************************************************************************
							  Function definitions
 ******************************************************************************/
 int  AACFindSyncWord(unsigned char *buf, int nBytes);
 
/**************************************************************************************
 * Function:    AACInitDecoder
 *
 * Description: allocate memory for platform-specific data
 *              clear all the user-accessible fields
 *              initialize SBR decoder if enabled
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      handle to AAC decoder instance, 0 if malloc fails
 **************************************************************************************/
AACInfo* AACParserLib_Create(void)
{
	AACInfo *pAACInfo;

	pAACInfo = AllocateBuffers();
	if (!pAACInfo)
		return 0;

	return pAACInfo;
}

/**************************************************************************************
 * Function:    AACFreeDecoder
 *
 * Description: free data allocated by AACParser_Create
 *
 * Inputs:      valid AAC Parser instance pointer
 *
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
void AACParserLib_Release(HAACParser hAACParser)
{
	if (!hAACParser)
		return;

	FreeBuffers(hAACParser);
}

/**************************************************************************************
 * Function:    AACFindSyncWord
 *
 * Description: locate the next byte-alinged sync word in the raw AAC stream
 *
 * Inputs:      buffer to search for sync word
 *              max number of bytes to search in buffer
 *
 * Outputs:     none
 *
 * Return:      offset to first sync word (bytes from start of buf)
 *              -1 if sync not found after searching nBytes
 **************************************************************************************/
#if 0
int AACFindSyncWord(unsigned char *buf, int nBytes)
{
	int i;

	/* find byte-aligned syncword (12 bits = 0xFFF) */
	for (i = 0; i < nBytes - 1; i++) {
		if ( (buf[i+0] & SYNCWORDH) == SYNCWORDH && (buf[i+1] & SYNCWORDL) == SYNCWORDL )
			return i;
	}
	
	return -1;
}
#endif


/**************************************************************************************
 * Function:    AACParser_Open
 *
 * Description: Open an AAC stream in first time to figure out what the file format is
 *
 * Inputs:      valid AAC parser instance pointer
 *              pointer to buffer of AAC data
 *              pointer to number of valid bytes remaining in inbuf
 *
 * Outputs:     File format and header information in pAACInfo
 *
 * Return:      0 if successful, error code (< 0) if error
 *
 * Notes:       inbuf pointer and bytesLeft are not updated. so if 
 *				ERR_AACPARSER_INDATA_UNDERFLOW is returned just call AACParserOpen again
 *				with more data in inbuf. Call AACParser_ADTSParsing to make
 *				real frame searching for ADTS stream
 **************************************************************************************/
int AACParserLib_Open(HAACParser pAACInfo, unsigned char **inbuf, int *bytesLeft)
{
	int err;
	int nbByteOffset, nBytesLeft;
	BSInfo bsi;
	unsigned char *inptr;
	
	if (!pAACInfo)
		return ERR_AACPARSER_NULL_POINTER;

	// stream have been opened
	if(pAACInfo->format != AAC_FF_Unknown)
		return ERR_AACPARSER_NONE;

	if (*bytesLeft < 8)
		return ERR_AACPARSER_INDATA_UNDERFLOW;
	
	inptr = *inbuf;
	nBytesLeft = *bytesLeft;

	// assume ADIF firstly
	if (IS_ADIF(inptr))
	{
		// unpack ADIF header 
		BS_InitPointer(&bsi, nBytesLeft, inptr);
		err = UnpackADIFHeader(pAACInfo, &bsi);
		if (err)
			return err;
		
		pAACInfo->format = AAC_FF_ADIF;
	} 
	else 
	{
		// assume ADTS, search for start of next frame 
		nbByteOffset = AACFindSyncWord((unsigned char *)inptr, (int)nBytesLeft);
		if (nbByteOffset < 0)
		{
			//syncword not found, update pointers, reserve one byte for next search
			*inbuf += *bytesLeft - 1;
			*bytesLeft = 1;
			return ERR_AACPARSER_INDATA_UNDERFLOW;
		}
		
		inptr += nbByteOffset;
		nBytesLeft -= nbByteOffset;	
		BS_InitPointer(&bsi, nBytesLeft, inptr);
		
		err = UnpackADTSHeader(pAACInfo, &bsi);
		if (err)
			return err;
		
		// save fixed header data, only 28 bits belongs to the fixed part
		memcpy(pAACInfo->ADTSheaderBackup, inptr+1, AAC_ADTS_FIXEDHEADER_COPY_SIZE);

		pAACInfo->format = AAC_FF_ADTS;
		
		//update pointers
		*bytesLeft = nBytesLeft;
		*inbuf = inptr;
	}
	
	// check for valid number of channels
	if (pAACInfo->nChans > AAC_MAX_NCHANS || pAACInfo->nChans <= 0)
		return ERR_AACPARSER_NCHANS_TOO_HIGH;
	
	if (bsi.nBitsLeft < 0)
		return ERR_AACPARSER_INDATA_UNDERFLOW;
	
	pAACInfo->outputSamps = pAACInfo->nChans << 10; // * 1024;
	
	return ERR_AACPARSER_NONE;
}
 
/**************************************************************************************
 * Function:    AACParser_ADTSParsing
 *
 * Description: parse AAC frame
 *
 * Inputs:      valid AAC decoder instance pointer (void*)
 *              double pointer to buffer of AAC data
 *              pointer to number of valid bytes remaining in inbuf
 *              pointer to outbuf, big enough to hold one frame of decoded PCM samples
 *                (outbuf must be double-sized if SBR enabled)
 *
 * Outputs:     Information of a AAC frame, updated inbuf pointer and bytesLeft
 *
 * Return:      0 if successful, error code (< 0) if error
 *
 * Notes:       inbuf pointer and bytesLeft are not updated until whole frame is
 *				successfully parsed, so if ERR_AACPARSER_INDATA_UNDERFLOW is returned
 *				just call AACParser_ADTSParsing again with more data in inbuf
 **************************************************************************************/
int AACParserLib_ADTSParsing(HAACParser pAACInfo, unsigned char **inbuf, int *bytesLeft)
{
	int nByteOffset, nBytesLeft;
	unsigned char *inptr;
	unsigned int tmpWord;
	
//	if (!pAACInfo)
//		return ERR_AACPARSER_NULL_POINTER;

//	if (pAACInfo->format != AAC_FF_ADTS) 
//		return ERR_AACPARSER_INVALID_ADTS_HEADER;
	
	inptr = *inbuf;
	nBytesLeft = *bytesLeft;

	//search for start of next frame
	nByteOffset = AACFindSyncWord(inptr, nBytesLeft);
	if (nByteOffset < 0)
	{
		//syncword not found, update pointers, reserve one byte for next search
		*inbuf += *bytesLeft - 1;
		*bytesLeft = 1;
		return ERR_AACPARSER_INDATA_UNDERFLOW;
	}

	inptr += nByteOffset;
	nBytesLeft -= nByteOffset;

	// check for fixed header
	if(CheckADTSHeader(pAACInfo->ADTSheaderBackup, inptr+1))
		return ERR_AACPARSER_INVALID_ADTS_HEADER;

	if (nBytesLeft < 7)
		return ERR_AACPARSER_INDATA_UNDERFLOW;

	// get varible Header info
	// only numRawDataBlocks and frameLength are needed
	tmpWord = (*(inptr+3) << 24) | (*(inptr+4) << 16) | (*(inptr+5) << 8) |	*(inptr+6);
	pAACInfo->fhADTS.numRawDataBlocks	= (tmpWord & 0x3) + 1;
	tmpWord <<= 6;
	pAACInfo->fhADTS.frameLength		= tmpWord >> 19;
	
//	BS_InitPointer(&bsi, nBytesLeft-3, inptr+3);
//	BS_GetBits(&bsi, 6);
//	pAACInfo->fhADTS.frameLength		= BS_GetBits(&bsi, 13);	// 43 bits
//	BS_GetBits(&bsi, 11);	// 54 bits
//	pAACInfo->fhADTS.numRawDataBlocks	= BS_GetBits(&bsi, 2) + 1;	// 56 bits

	if(nBytesLeft < pAACInfo->fhADTS.frameLength)
	{
		*bytesLeft = nBytesLeft;
		*inbuf = inptr;
		return ERR_AACPARSER_INDATA_UNDERFLOW;
	}

	pAACInfo->frameCount += pAACInfo->fhADTS.numRawDataBlocks;
	pAACInfo->nDataCount += pAACInfo->fhADTS.frameLength;

	//update pointers
	*bytesLeft = nBytesLeft - pAACInfo->fhADTS.frameLength;
	*inbuf = inptr + pAACInfo->fhADTS.frameLength;
	
	return ERR_AACPARSER_NONE;
}
