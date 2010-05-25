/******************************************************************************
               (C) copyright (2006)  Vimicro  All Rights Reserved
 ******************************************************************************/

/******************************************************************************
 File:	bitstream.h
 Date:	2006-3-6   10:43
 ------------------------------------------------------------------------------
 Description:
	
 ******************************************************************************/

/******************************************************************************
                      C H A N G E   R E C O R D

 #   Date		     Author			Description
 --  ----------	 ------------------  --------------------------------------
 01  2006-3-6	Xiaocheng Wang		Created this file
 ******************************************************************************/

#ifndef __AACPARSER_H__
#define __AACPARSER_H__

#if defined(__cplusplus)
extern "C"{
#endif
	
/******************************************************************************
							  Include files
 ******************************************************************************/
#include "aacparselib.h"
#include "bitstream.h"

/******************************************************************************
							  Macro definitions
 ******************************************************************************/
#define IS_ADIF(p)		((p)[0] == 'A' && (p)[1] == 'D' && (p)[2] == 'I' && (p)[3] == 'F')
#define GET_ELE_ID(p)	((AACElementID)(*(p) >> (8-NUM_SYN_ID_BITS)))

#define CHAN_ELEM_IS_CPE(x)			(((x) & 0x10) >> 4)  // bit 4 = SCE/CPE flag 
#define CHAN_ELEM_GET_TAG(x)		(((x) & 0x0f) >> 0)  // bits 3-0 = instance tag 

#define CHAN_ELEM_SET_CPE(x)		(((x) & 0x01) << 4)  // bit 4 = SCE/CPE flag 
#define CHAN_ELEM_SET_TAG(x)		(((x) & 0x0f) << 0)  // bits 3-0 = instance tag 

// 12-bit syncword 
#define	SYNCWORDH			0xff
#define	SYNCWORDL			0xf0

#define ADTS_HEADER_BYTES	7

#define NUM_SAMPLE_RATES	12
#define NUM_DEF_CHAN_MAPS	8

#define MAX_NUM_FCE                     15
#define MAX_NUM_SCE                     15
#define MAX_NUM_BCE                     15
#define MAX_NUM_LCE                      3
#define MAX_NUM_ADE                      7
#define MAX_NUM_CCE                     15

/******************************************************************************
							  Type definitions
 ******************************************************************************/

//sizeof(ProgConfigElement) = 82 bytes (if KEEP_PCE_COMMENTS not defined) 
typedef struct _ProgConfigElement 
{
    unsigned char elemInstTag;                    /* element instance tag */
    unsigned char profile;                        /* 0 = main, 1 = LC, 2 = SSR, 3 = reserved */
    unsigned char sampRateIdx;                    /* sample rate index range = [0, 11] */
    unsigned char numFCE;                         /* number of front channel elements (max = 15) */
    unsigned char numSCE;                         /* number of side channel elements (max = 15) */
    unsigned char numBCE;                         /* number of back channel elements (max = 15) */
    unsigned char numLCE;                         /* number of LFE channel elements (max = 3) */
    unsigned char numADE;                         /* number of associated data elements (max = 7) */
    unsigned char numCCE;                         /* number of valid channel coupling elements (max = 15) */
    unsigned char monoMixdown;                    /* mono mixdown: bit 4 = present flag, bits 3-0 = element number */
    unsigned char stereoMixdown;                  /* stereo mixdown: bit 4 = present flag, bits 3-0 = element number */
    unsigned char matrixMixdown;                  /* matrix mixdown: bit 4 = present flag, bit 3 = unused, 
                                                     bits 2-1 = index, bit 0 = pseudo-surround enable */
    unsigned char fce[MAX_NUM_FCE];               /* front element channel pair: bit 4 = SCE/CPE flag, bits 3-0 = inst tag */
    unsigned char sce[MAX_NUM_SCE];               /* side element channel pair: bit 4 = SCE/CPE flag, bits 3-0 = inst tag */
    unsigned char bce[MAX_NUM_BCE];               /* back element channel pair: bit 4 = SCE/CPE flag, bits 3-0 = inst tag */
    unsigned char lce[MAX_NUM_LCE];               /* instance tag for LFE elements */
    unsigned char ade[MAX_NUM_ADE];               /* instance tag for ADE elements */
    unsigned char cce[MAX_NUM_BCE];               /* channel coupling elements: bit 4 = switching flag, bits 3-0 = inst tag */
} ProgConfigElement;

/******************************************************************************
							  Constant definitions
 ******************************************************************************/

/******************************************************************************
							  Function prototypes
 ******************************************************************************/
extern void * me_malloc(unsigned int size);
extern void * me_free(void * buf);

AACInfo *AllocateBuffers(void);
void FreeBuffers(AACInfo *aacInfo);
void ClearBuffer(void *buf, int nBytes);

int  CheckADTSHeader(unsigned char *pADTS1, unsigned char *pADTS2);
int  UnpackADTSHeader(AACInfo *aacInfo, BSInfo *bsi);
int  ParseADTSHeader( AACInfo *aacInfo, BSInfo *bsi);
int  UnpackADIFHeader(AACInfo *aacInfo, BSInfo *bsi);

#if defined(__cplusplus)
}
#endif

#endif	// __AACPARSER_H__