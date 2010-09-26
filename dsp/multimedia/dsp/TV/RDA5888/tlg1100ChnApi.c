/*****************************************************************************
* FILENAME
* tlg1100ChnApi.c
*
*
* ABSTRACT
* This file contains the code for the TLG1100 Ultra Low Power NTSC/PAL
* Tuner/Decoder Front End Application Programmer's Interface library.
*
* $Revision: 1.40 $
*
* (c) 2006 Telegent Systems
*****************************************************************************/


#include "tlg1100Hal.h"
#include "rda5888adp.h"

extern int tlg_init_done;

/*****************************************************************************
* TLG_SetChnStandard
*
* 
*
*
*****************************************************************************/

TLG1100DLL_API int TLG_SetChnStandard(uint32 base_addr, uint16 mode)
{
	RDA_SetChnStandard(base_addr, mode);
}

#define CHANNEL_EXIST      1
#define CHANNEL_NOT_EXIST  0

/*****************************************************************************
* TLG_ScanTVChn
*
* 
*
*
*****************************************************************************/
TLG1100DLL_API int TLG_ScanTVChn(uint32 base_addr, uint32 center_freq_hz)
{
	int res = CHANNEL_NOT_EXIST;

	res = RDA_ScanTVChn(base_addr, center_freq_hz);

	return res;
}
