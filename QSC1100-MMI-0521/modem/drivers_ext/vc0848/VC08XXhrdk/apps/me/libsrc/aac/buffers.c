/******************************************************************************
               (C) copyright (2006)  Vimicro  All Rights Reserved
 ******************************************************************************/
/******************************************************************************
 File:	buffers.c
 Date:	2006-3-7   10:34
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
#include <stdlib.h>
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
/******************************************************************************
							  Function definitions
 ******************************************************************************/

/**************************************************************************************
 * Function:    ClearBuffer
 *
 * Description: fill buffer with 0's
 *
 * Inputs:      pointer to buffer
 *              number of bytes to fill with 0
 *
 * Outputs:     cleared buffer
 *
 * Return:      none
 *
 * Notes:       slow, platform-independent equivalent to memset(buf, 0, nBytes)
 **************************************************************************************/
void ClearBuffer(void *buf, int nBytes)
{
	int i;
	unsigned char *cbuf = (unsigned char *)buf;

	for (i = 0; i < nBytes; i++)
		cbuf[i] = 0;

	return;
}

/**************************************************************************************
 * Function:    AllocateBuffers
 *
 * Description: allocate all the memory needed for the AACInfo
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      pointer to AACInfo structure, cleared to all 0's (except for
 *                pointer to platform-specific data structure)
 *
 * Notes:       if one or more mallocs fail, function frees any buffers already
 *                allocated before returning
 **************************************************************************************/
AACInfo *AllocateBuffers(void)
{
	AACInfo *aacInfo;

	aacInfo = (AACInfo *)aac_parser_malloc(sizeof(AACInfo));
	AAC_PARSER_TRACE_MSG();	//add by amanda deng for testing memory lost
	if (!aacInfo)
		return 0;
	ClearBuffer(aacInfo, sizeof(AACInfo));
	
	aacInfo->pce = (void*)aac_parser_malloc(sizeof(ProgConfigElement));
	AAC_PARSER_TRACE_MSG();	//add by amanda deng for testing memory lost
	if(!aacInfo->pce)
	{
		FreeBuffers(aacInfo);
		return 0;
	}
	
	ClearBuffer(aacInfo->pce, sizeof(ProgConfigElement));
	return aacInfo;
}

#ifndef SAFE_FREE
#define SAFE_FREE(x)	{if (x)	me_free(x);	(x) = 0;}	/* helper macro */
#endif

/**************************************************************************************
 * Function:    FreeBuffers
 *
 * Description: frees all the memory used by the AACInfo
 *
 * Inputs:      pointer to initialized AACInfo structure
 *
 * Outputs:     none
 *
 * Return:      none
 *
 * Notes:       safe to call even if some buffers were not allocated (uses SAFE_FREE)
 **************************************************************************************/
void FreeBuffers(AACInfo *aacInfo)
{
	AAC_PARSER_FREE_MSG(("address=0x%x\n", (unsigned int)aacInfo->pce));
	AAC_PARSER_FREE_MSG(("address=0x%x\n", (unsigned int)aacInfo));
	SAFE_FREE(aacInfo->pce);
	AAC_PARSER_TRACE_MSG();	//add by amanda deng for testing memory lost
	SAFE_FREE(aacInfo);
	AAC_PARSER_TRACE_MSG();	//add by amanda deng for testing memory lost
}





