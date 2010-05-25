/******************************************************************************
               (C) copyright (2006)  Vimicro  All Rights Reserved
 ******************************************************************************/
/******************************************************************************
 File:	filefmt.c
 Date:	2006-3-7   10:36
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
#include "aacparser.h" 

/******************************************************************************
							  Macro definitions
 ******************************************************************************/
/******************************************************************************
							  Type definitions
 ******************************************************************************/
/******************************************************************************
							  Variable definitions
 ******************************************************************************/
//sample rates (table 4.5.1) 
const int sampRateTab[NUM_SAMPLE_RATES] = 
{
    96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025,  8000
};

//channel mapping (table 1.6.3.4) (-1 = unknown, so need to determine mapping based on rules in 8.5.1) 
const int channelMapTab[NUM_DEF_CHAN_MAPS] = {	-1, 1, 2, 3, 4, 5, 6, 8 };

/******************************************************************************
							  Function definitions
 ******************************************************************************/

/**************************************************************************************
 * Function:    DecodeProgramConfigElement
 *
 * Description: decode one PCE and caculate the channel info and check the sampRateIdx
 *
 * Inputs:      BitStreamInfo struct pointing to start of PCE (14496-3, table 4.4.2) 
 *
 * Outputs:     filled-in ProgConfigElement struct
 *              updated BitStreamInfo struct
 *
 * Return:      0 if successful, error code (< 0) if error
**************************************************************************************/
static int DecodeProgramConfigElement(ProgConfigElement *pce, BSInfo *bsi, AACInfo *pAACInfo, int iIndex)
{
	int i,j;
	
	pce->elemInstTag =   BS_GetBits(bsi, 4);
	pce->profile =       BS_GetBits(bsi, 2);
	pce->sampRateIdx =   BS_GetBits(bsi, 4);
	pce->numFCE =        BS_GetBits(bsi, 4);
	pce->numSCE =        BS_GetBits(bsi, 4);
	pce->numBCE =        BS_GetBits(bsi, 4);
	pce->numLCE =        BS_GetBits(bsi, 2);
	pce->numADE =        BS_GetBits(bsi, 3);
	pce->numCCE =        BS_GetBits(bsi, 4);
	
	pce->monoMixdown = BS_GetBits(bsi, 1) << 4;			/* present flag */
	if (pce->monoMixdown)
		pce->monoMixdown |= BS_GetBits(bsi, 4);			/* element number */
	
	pce->stereoMixdown = BS_GetBits(bsi, 1) << 4;		/* present flag */
	if (pce->stereoMixdown)
		pce->stereoMixdown  |= BS_GetBits(bsi, 4);		/* element number */
	
	pce->matrixMixdown = BS_GetBits(bsi, 1) << 4;		/* present flag */
	if (pce->matrixMixdown) {
		pce->matrixMixdown  |= BS_GetBits(bsi, 2) << 1;	/* index */
		pce->matrixMixdown  |= BS_GetBits(bsi, 1);		/* pseudo-surround enable */
	}
	
	for (i = 0; i < pce->numFCE; i++) {
		pce->fce[i]  = BS_GetBits(bsi, 1) << 4;			/* is_cpe flag */
		pce->fce[i] |= BS_GetBits(bsi, 4);				/* tag select */
	}
	
	for (i = 0; i < pce->numSCE; i++) {
		pce->sce[i]  = BS_GetBits(bsi, 1) << 4;			/* is_cpe flag */
		pce->sce[i] |= BS_GetBits(bsi, 4);				/* tag select */
	}
	
	for (i = 0; i < pce->numBCE; i++) {
		pce->bce[i]  = BS_GetBits(bsi, 1) << 4;			/* is_cpe flag */
		pce->bce[i] |= BS_GetBits(bsi, 4);				/* tag select */
	}
	
	for (i = 0; i < pce->numLCE; i++)
		pce->lce[i] = BS_GetBits(bsi, 4);				/* tag select */
	
	for (i = 0; i < pce->numADE; i++)
		pce->ade[i] = BS_GetBits(bsi, 4);				/* tag select */
	
	for (i = 0; i < pce->numCCE; i++) {
		pce->cce[i]  = BS_GetBits(bsi, 1) << 4;			/* independent/dependent flag */
		pce->cce[i] |= BS_GetBits(bsi, 4);				/* tag select */
	}
	
	//////////////////////////////////////////////////////////////////////////////
	// gathering aacinfo here without saving each Program Config Element	
	if(0 == iIndex)
	{
		// GetSampleRateIdxADIF(), for pce[0]
		pAACInfo->sampRateIdx = pce->sampRateIdx;

		// GetNumChannelsADIF(),  for pce[0], reset channel counter
		pAACInfo->nChans = 0;		
	}
	else
	{
		// GetSampleRateIdxADIF(), for other pce  
		if( pAACInfo->sampRateIdx != pce->sampRateIdx)
			pAACInfo->sampRateIdx = -1;
	}
	
	// for now: only support LC, no channel coupling
	if (pce->profile != AAC_PROFILE_LC || pce->numCCE > 0)
		pAACInfo->nChans = -1;
	else
	{
		// add up number of channels in all channel elements (assume all single-channel) 
		pAACInfo->nChans += pce->numFCE;
		pAACInfo->nChans += pce->numSCE;
		pAACInfo->nChans += pce->numBCE;
		pAACInfo->nChans += pce->numLCE;
		
		// add one more for every element which is a channel pair 
		for (j = 0; j < pce->numFCE; j++) 
		{
			if (CHAN_ELEM_IS_CPE(pce->fce[j]))
				pAACInfo->nChans++;
		}

		for (j = 0; j < pce->numSCE; j++) 
		{
			if (CHAN_ELEM_IS_CPE(pce->sce[j]))
				pAACInfo->nChans++;
		}

		for (j = 0; j < pce->numBCE; j++) 
		{
			if (CHAN_ELEM_IS_CPE(pce->bce[j]))
				pAACInfo->nChans++;
		}
	}
	
	BS_ByteAlign(bsi);
	
	// eat comment bytes and throw away 
	i = BS_GetBits(bsi, 8);
	while (i--)
		BS_GetBits(bsi, 8);
	
	return 0;
}

int CheckADTSHeader(unsigned char *pADTS1, unsigned char *pADTS2)
{
	//////////////////////////////////////////////////////////////////////////////
	// fixed fields - should not change from frame to frame 
	// check 20 bits	
	
	// first 16 bits
	if (*pADTS1++ != *pADTS2++)
		return 1;
	if (*pADTS1++ != *pADTS2++)
		return 1;

	// 4 bits
	if ( (*pADTS1 >> 4) != (*pADTS2 >> 4) )
		return 1;

	return 0;
}

 /**************************************************************************************
 * Function:    UnpackADTSHeader
 *
 * Description: parse the ADTS frame header and get the AAC stream information
 *
 * Inputs:      valid AACInfo struct
 *              double pointer to buffer with complete ADTS frame header (byte aligned)
 *              header size = 7 bytes, plus 2 if CRC
 *
 * Outputs:     filled in ADTS struct
 *              updated buffer pointer
 *              updated bit offset
 *              updated number of available bits
 *
 * Return:      0 if successful, error code (< 0) if error
 **************************************************************************************/
int UnpackADTSHeader(AACInfo *pAACInfo, BSInfo *bsi)
{
	ADTSHeader *fhADTS = &pAACInfo->fhADTS;

	// verify that first 12 bits of header are syncword 
//	if (BS_GetBits(bsi, 12) != 0x0fff)					// 12 bits
//		return ERR_AACPARSER_INVALID_ADTS_HEADER;

	// this has been check by AACFindSyncWord( )
	BS_GetBits(bsi, 12);
		
	// fixed fields - should not change from frame to frame 
	fhADTS->id =               BS_GetBits(bsi, 1);		// 13 bits
	fhADTS->layer =            BS_GetBits(bsi, 2);		// 15 bits
	fhADTS->protectBit =       BS_GetBits(bsi, 1);		// 16 bits
	fhADTS->profile =          BS_GetBits(bsi, 2);		// 18 bits
	fhADTS->sampRateIdx =      BS_GetBits(bsi, 4);		// 22 bits
	fhADTS->privateBit =       BS_GetBits(bsi, 1);		// 23 bits
	fhADTS->channelConfig =    BS_GetBits(bsi, 3);		// 26 bits
	fhADTS->origCopy =         BS_GetBits(bsi, 1);		// 27 bits
	fhADTS->home =             BS_GetBits(bsi, 1);		// 28 bits

	// variable fields - can change from frame to frame 
	fhADTS->copyBit =          BS_GetBits(bsi, 1);		// 29 bits
	fhADTS->copyStart =        BS_GetBits(bsi, 1);		// 30 bits
	
//	fhADTS->copyBit =          GetBits(&bsi, 1);
//	fhADTS->copyStart =        GetBits(&bsi, 1);
	BS_GetBits(bsi, 2);

	fhADTS->frameLength =      BS_GetBits(bsi, 13);		// 43 bits
	fhADTS->bufferFull =       BS_GetBits(bsi, 11);		// 54 bits	
	fhADTS->numRawDataBlocks = BS_GetBits(bsi, 2) + 1;	// 56 bits

	if(bsi->nBitsLeft < 0)
		return ERR_AACPARSER_INDATA_UNDERFLOW;

	if (!pAACInfo->useImpChanMap)
		pAACInfo->nChans = channelMapTab[fhADTS->channelConfig];
	
	pAACInfo->sampRateIdx = fhADTS->sampRateIdx;
	
	// fill in user-accessible data
	pAACInfo->bitRate = 0;			// unknow
	pAACInfo->sampRateCore = pAACInfo->sampRate = sampRateTab[pAACInfo->sampRateIdx];
	pAACInfo->profile = pAACInfo->fhADTS.profile;

	return ERR_AACPARSER_NONE;
}

/**************************************************************************************
 * Function:    UnpackADIFHeader
 *
 * Description: parse the ADIF file header and initialize decoder state
 *
 * Inputs:      valid AACInfo struct
 *              double pointer to buffer with complete ADIF header 
 *                (starting at 'A' in 'ADIF' tag)
 *              pointer to bit offset
 *              pointer to number of valid bits remaining in inbuf
 *
 * Outputs:     filled-in ADIF struct
 *              updated buffer pointer
 *              updated bit offset
 *              updated number of available bits
 *
 * Return:      0 if successful, error code (< 0) if error
 **************************************************************************************/
int UnpackADIFHeader(AACInfo *pAACInfo, BSInfo *bsi)
{
	int i;
	ADIFHeader *fhADIF;
	ProgConfigElement *pce;

	// validate pointers 
	if (!pAACInfo)
		return ERR_AACPARSER_NULL_POINTER;

	// unpack ADIF file header 
	fhADIF = &(pAACInfo->fhADIF);
	pce = (ProgConfigElement *)pAACInfo->pce;

	// verify that first 32 bits of header are "ADIF" 
	if (BS_GetBits(bsi, 8) != 'A' || BS_GetBits(bsi, 8) != 'D' || BS_GetBits(bsi, 8) != 'I' || BS_GetBits(bsi, 8) != 'F')
		return ERR_AACPARSER_INVALID_ADIF_HEADER;

	// read ADIF header fields 
	fhADIF->copyBit = BS_GetBits(bsi, 1);
	if (fhADIF->copyBit) {
		for (i = 0; i < ADIF_COPYID_SIZE; i++)
			fhADIF->copyID[i] = BS_GetBits(bsi, 8);
	}

	fhADIF->origCopy = BS_GetBits(bsi, 1);
	fhADIF->home =     BS_GetBits(bsi, 1);

	fhADIF->bsType =   BS_GetBits(bsi, 1);
	fhADIF->bitRate =  BS_GetBits(bsi, 23);
	fhADIF->numPCE =   BS_GetBits(bsi, 4) + 1;	// add 1 (so range = [1, 16]) 
	if (fhADIF->bsType == 0)
		fhADIF->bufferFull = BS_GetBits(bsi, 20);

	// parse all program config elements 
	for (i = 0; i < fhADIF->numPCE; i++)
		DecodeProgramConfigElement(pce, bsi, pAACInfo, i);

	// byte align 
//	BS_ByteAlign(bsi);

	// check validity of header 
	if (pAACInfo->nChans < 0 || pAACInfo->sampRateIdx < 0 || pAACInfo->sampRateIdx >= NUM_SAMPLE_RATES)
		return ERR_AACPARSER_INVALID_ADIF_HEADER;
								
	// fill in user-accessible data 
	pAACInfo->bitRate = fhADIF->bitRate;
	pAACInfo->profile = pce->profile;
	pAACInfo->sampRateCore = pAACInfo->sampRate = sampRateTab[pAACInfo->sampRateIdx];


	if(bsi->nBitsLeft < 0)
		return ERR_AACPARSER_INDATA_UNDERFLOW;

	return ERR_AACPARSER_NONE;
}