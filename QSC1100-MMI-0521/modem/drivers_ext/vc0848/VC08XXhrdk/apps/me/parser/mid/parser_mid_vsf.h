#ifndef _PARSER_MID_VSF_H
#define _PARSER_MID_VSF_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "def.h"
#include "..\..\libsrc\ima\ima_adpcm_codec.h"

#define PARALEN 26
#define MAX_ZONE_LEN 100

typedef struct  {
	UINT32 OldLoopAddress;
	UINT32 NewLoopAddress;
}INSTRUMENT_MAP;

typedef struct{
    UINT32  aStartaddress;//the absolute address of the start sample
	ima_ADPCMstate loopstate;// adpcm state of the loop start sample
}AdpcmWave_InstrumentMap;

typedef struct _Download_Zone
{
	//UINT16	bBankID;                      /* bank number 0: common instrument, 128: drum instrument*/
	UINT8   bBankID;
	UINT8	bTimbre;                           /* preset number (0--127)*/
	UINT8 	bLowNote;                          /*the lowest note that use this sample           */
	UINT8 	bHighNote;	                       /*the highest note that use this sample          */
	UINT8 	bSamplePitch;                      /*the original pitch value                       */	
	SINT8 	sbPitchCorrect;	                   /*the pitch when using pitch correction          */
	UINT16 	wStartOffset;                      /*negative value, from start to loop, in samples,*/ 
	/* * (-1) before use it                          */	
	UINT16 	wEndOffset;	                       /* Sample End point in the bank                  */
	UINT32 	dLoopAddress;                      /* Absolute loop address in the bank             */	
	UINT16 	wSampleRate;                       /* The sample rate of this sample                */	
	UINT16 	bACounter;                         /* This counter will be load in the hardware     */ 	
	UINT16 	bDCounter;                         /* Delay counter in the envelope                 */	
	UINT16 	bRCounter;                         /* Release counter in the envelope               */	
	UINT16 	wSLevel;                           /* The sustain Level   in the envelope           */	
	UINT16 	wAttenuation;                      /* The Attenuation in he envelope                */		
	UINT8 	bBoolCtrl;                         /* The LOOPEN, SIGNCTRL and BITCTRL control      */
	UINT8	bLfoCounter;                       /* The LFO counter in the envelope               */
	//PDownload_Sample ds;
	
} Download_Zone, *PDownload_Zone;

ME_U32 parser_mid_get_bandk_header(UINT8 bBankID, UINT8 bTimbre, UINT8 bNote,void *pvm_input);
ME_U32 parser_mid_get_bank_data(VM_MIDI_PARSER_INFO *file);

#ifdef __cplusplus
}
#endif

#endif
