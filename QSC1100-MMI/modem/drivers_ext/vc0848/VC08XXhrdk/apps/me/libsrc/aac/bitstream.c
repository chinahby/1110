/******************************************************************************
               (C) copyright (2006)  Vimicro  All Rights Reserved
 ******************************************************************************/

/******************************************************************************
 File:	bitstream.c
 Date:	2006-3-7   10:35
 ------------------------------------------------------------------------------
 Description:
	 
 ******************************************************************************/

/******************************************************************************
                      C H A N G E   R E C O R D

 #   Date		     Author			Description
 --  ----------	 ------------------  --------------------------------------
 01  2006-3-7	Xiaocheng Wang		Created this file
 ******************************************************************************/
/******************************************************************************
							  Include files
 ******************************************************************************/
#include "bitstream.h"

/******************************************************************************
							  Macro definitions
 ******************************************************************************/
/******************************************************************************
							  Type definitions
 ******************************************************************************/
/******************************************************************************
							  Variable definitions
 ******************************************************************************/
/******************************************************************************
							  Function definitions
 ******************************************************************************/
/**************************************************************************************
 * Function:    BS_InitPointer
 *
 * Description: initialize bitstream reader
 *
 * Inputs:      pointer to BSInfo struct
 *              number of bytes in bitstream
 *              pointer to byte-aligned buffer of data to read from
 *
 * Outputs:     initialized bitstream info struct
 *
 * Return:      none
 **************************************************************************************/
void BS_InitPointer(BSInfo *pBSInfo, int nBytes, unsigned char *buf)
{
	pBSInfo->pCurrPtr = pBSInfo->pBuffPtr = buf;
	pBSInfo->iCache = 0;			// 4-byte unsigned int
	pBSInfo->nBitsLeft = 0;			// i.e. zero bits in cache
	pBSInfo->nBytesNum = nBytes;
}

/**************************************************************************************
 * Function:    FillCache
 *
 * Description: read new data from bitstream buffer into 32-bit cache
 *
 * Inputs:      pointer to initialized BSInfo struct
 *
 * Outputs:     updated bitstream info struct
 *
 * Return:      none
 *
 * Notes:       only call when iCache is completely drained (resets bitOffset to 0)
 *              always loads 4 new bytes except when pBSInfo->nBytesNum < 4 (end of buffer)
 *              stores data as big-endian in cache, regardless of machine endian-ness
 **************************************************************************************/
static __inline void FillCache(BSInfo *pBSInfo)
{
	int nBytesNum  = pBSInfo->nBytesNum;
	int nFillBytes = 4;
	int iLeftShift = 0;
	
	if(nBytesNum < 4)
	{
		nFillBytes = nBytesNum;
		iLeftShift = (4 - nBytesNum) << 3;
	}
	
	pBSInfo->nBitsLeft = (nFillBytes << 3);
	pBSInfo->nBytesNum -= nFillBytes;
	
	// optimize for common case, independent of machine endian-ness
	pBSInfo->iCache = *pBSInfo->pCurrPtr++;
	nFillBytes--;
	while(nFillBytes--)
	{
		pBSInfo->iCache = (pBSInfo->iCache << 8) | *pBSInfo->pCurrPtr++;
	}
	
	pBSInfo->iCache <<= iLeftShift;	// always move the bits to the highest position in the word
}

/**************************************************************************************
 * Function:    ShowNextWord
 *
 * Description: get bits from bitstream, do not advance bitstream pointer
 *
 * Inputs:      pointer to initialized BSInfo struct
 *              number of bits to get from bitstream
 *
 * Outputs:     none (state of BSInfo struct left unchanged) 
 *
 * Return:      the next nBits bits of data from bitstream buffer
 *
 * Notes:       nBits must be in range [0, 31], nBits outside this range masked by 0x1f
 *              for speed, does not indicate error if you overrun bit buffer 
 *              if nBits == 0, returns 0
 **************************************************************************************/
static unsigned int ShowNextWord(BSInfo *pBSInfo, int nBits, unsigned int iCache)
{
	unsigned char *pDataPtr = pBSInfo->pCurrPtr;
	
	while (nBits > 0) 
	{
		iCache <<= 8;
		if (pDataPtr < pBSInfo->pCurrPtr + pBSInfo->nBytesNum)
			iCache |= (unsigned int)*pDataPtr++;
		nBits -= 8;
	}

	nBits = -nBits;
	iCache >>= nBits;

	return iCache;
}

/**************************************************************************************
 * Function:    BS_ShowBits
 *
 * Description: get bits from bitstream, do not advance bitstream pointer
 *
 * Inputs:      pointer to initialized BSInfo struct
 *              number of bits to get from bitstream
 *
 * Outputs:     none (state of BSInfo struct left unchanged) 
 *
 * Return:      the next nBits bits of data from bitstream buffer
 *
 * Notes:       nBits must be in range [0, 31], nBits outside this range masked by 0x1f
 *              for speed, does not indicate error if you overrun bit buffer 
 *              if nBits == 0, returns 0
 **************************************************************************************/
unsigned int BS_ShowBits(BSInfo *pBSInfo, int nBits)
{
	unsigned int uiData;

	if(nBits <= pBSInfo->nBitsLeft)
	{
		uiData = ( pBSInfo->iCache>> (32 - nBits));
	}
	else
	{
		// if we cross an int boundary, read next 4 bytes in buffer
		uiData = pBSInfo->iCache >> (32-pBSInfo->nBitsLeft);
		uiData = ShowNextWord(pBSInfo, nBits-pBSInfo->nBitsLeft, uiData);
	}	
	
	return uiData;
}

/**************************************************************************************
 * Function:    BS_MoveBits
 *
 * Description: move bitstream pointer ahead
 *
 * Inputs:      pointer to initialized BSInfo struct
 *              number of bits to advance bitstream
 *
 * Outputs:     updated bitstream info struct
 *
 * Return:      none
 *
 * Notes:       generally used following GetBitsNoAdvance(pBSInfo, maxBits)
 **************************************************************************************/
void BS_MoveBits(BSInfo *pBSInfo, int nBits)
{

	if (nBits > pBSInfo->nBitsLeft) 
	{
		nBits -= pBSInfo->nBitsLeft;
		FillCache(pBSInfo);
	}
	pBSInfo->iCache <<= nBits;
	pBSInfo->nBitsLeft -= nBits;
}

/**************************************************************************************
 * Function:    BS_GetBits
 *
 * Description: get bits from bitstream, advance bitstream pointer
 *
 * Inputs:      pointer to initialized BSInfo struct
 *              number of bits to get from bitstream
 *
 * Outputs:     updated bitstream info struct
 *
 * Return:      the next nBits bits of data from bitstream buffer
 *
 * Notes:       nBits must be in range [0, 31], nBits outside this range masked by 0x1f
 *              for speed, does not indicate error if you overrun bit buffer 
 *              if nBits == 0, returns 0
 **************************************************************************************/
unsigned int BS_GetBits(BSInfo *pBSInfo, int nBits)
{
	unsigned int uiData;
	int	nBitsLeft;
	
	nBitsLeft = 32 - nBits;
	uiData = pBSInfo->iCache >> nBitsLeft;

	if (nBits > pBSInfo->nBitsLeft) 
	{
		// fill low bits with next word
		nBitsLeft += pBSInfo->nBitsLeft;
		nBits -= pBSInfo->nBitsLeft;
		FillCache(pBSInfo);
		uiData = uiData | (pBSInfo->iCache >> nBitsLeft);		
	}

	pBSInfo->iCache <<= nBits;
	pBSInfo->nBitsLeft -= nBits;
	
	return uiData;
}

/**************************************************************************************
 * Function:    BS_ByteAlign
 *
 * Description: bump bitstream pointer to start of next byte
 *
 * Inputs:      pointer to initialized BSInfo struct
 *
 * Outputs:     byte-aligned bitstream BSInfo struct
 *
 * Return:      none
 *
 * Notes:       if bitstream is already byte-aligned, do nothing
 **************************************************************************************/
void BS_ByteAlign(BSInfo *pBSInfo)
{
	int offset;

	offset = pBSInfo->nBitsLeft & 0x07;
	if(offset)
		BS_MoveBits(pBSInfo, offset);
}
