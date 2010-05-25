#include "parser_mid.h"
#include "parser_mid_vsf.h"
#include "..\..\..\..\inc\mmd_file_driver.h"
#if ME_KM_PARSER_MIDI_EN

#include "soundbankdata.h"
#include "soundbankheader.h"

static UINT32 Mapping_adpcm(VM_MIDI_INPUT *vm_input,UINT32 wavenum)
{
	Download_Zone* Down_Zone=(Download_Zone*)vm_input->pBankheader;
	Download_Zone* pHander;
	AdpcmWave_InstrumentMap *pAdpcmMapping=(AdpcmWave_InstrumentMap*)vm_input->pAdpcmMapping;
	AdpcmWave_InstrumentMap *pThisWave;
	UINT32 Zone_Len=vm_input->ZoneLen;
	SINT8	*pNewAdpcmSampleData=(SINT8*)vm_input->pBankData;
	SINT8   *pAdpcmSampleData;
	UINT32	j,i,sampleLen;
	INSTRUMENT_MAP *instrument_map;
	//ima_ADPCMstate		state;
	UINT32 instrument_map_index;
	SINT8* pbSampleData=(SINT8*)BankDataArray;
	UINT32 LoopAddress,NewOffset;
	
	instrument_map=(INSTRUMENT_MAP*)me_malloc(wavenum*sizeof(INSTRUMENT_MAP));
	if(instrument_map==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	
	instrument_map_index=0;
	NewOffset=0;
	
	for(i=0;i<Zone_Len;i++)
	{
		pHander=&(Down_Zone[i]);
		LoopAddress=pHander->dLoopAddress;
		for(j=0;j<instrument_map_index;j++)
		{
			if(LoopAddress==instrument_map[j].OldLoopAddress)
			{
				LoopAddress=instrument_map[j].NewLoopAddress;
				break;
			}
		}
		
		if(j==instrument_map_index)
		{
			pAdpcmSampleData = pbSampleData + ((LoopAddress-pHander->wStartOffset)>>1);
			sampleLen = (UINT32)pHander->wEndOffset + pHander->wStartOffset;
			if(sampleLen%8!=0) 	sampleLen = (( ( sampleLen>>3) +1 )<<3);
			
			pThisWave=&pAdpcmMapping[j];
			instrument_map[j].OldLoopAddress=LoopAddress;
			//LoopAddress=(UINT32)pThisWave;
			LoopAddress=j;
			instrument_map[j].NewLoopAddress=LoopAddress;
			
			sampleLen>>=1; //convert from sample number to byte number
			MMD_MemCpy(pNewAdpcmSampleData,pAdpcmSampleData,sampleLen);
			MMD_MemCpy(&pThisWave->loopstate,pAdpcmSampleData+sampleLen,sizeof(ima_ADPCMstate));
	
			pThisWave->aStartaddress=NewOffset;//byte offset
			NewOffset+=sampleLen;
			pNewAdpcmSampleData+=sampleLen;
			instrument_map_index++;		
		}
		
		pHander->dLoopAddress=LoopAddress;
	}
	
	me_free(instrument_map);
	return ME_SUCCESS;
}

static UINT32 Decoder_adpcm(VM_MIDI_INPUT *vm_input,UINT32 wavenum)
{
	Download_Zone* Down_Zone=(Download_Zone*)vm_input->pBankheader;
	Download_Zone* pHander;
	UINT32 Zone_Len=vm_input->ZoneLen;
	SINT16	*pPcmSampleData=(SINT16*)vm_input->pBankData;
	SINT8	*pAdpcmSampleData;
	UINT32	j,i,sampleLen;
	INSTRUMENT_MAP *instrument_map;
	ima_ADPCMstate		state;
	UINT32 instrument_map_index;
	SINT8* pbSampleData=(SINT8*)BankDataArray;
	UINT32 LoopAddress,NewOffset;

	instrument_map=(INSTRUMENT_MAP*)me_malloc(wavenum*sizeof(INSTRUMENT_MAP));
	if(instrument_map==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}

	instrument_map_index=0;
	NewOffset=0;

	for(i=0;i<Zone_Len;i++)
	{
		pHander=&(Down_Zone[i]);
		LoopAddress=pHander->dLoopAddress;
		for(j=0;j<instrument_map_index;j++)
		{
			if(LoopAddress==instrument_map[j].OldLoopAddress)
			{
				LoopAddress=instrument_map[j].NewLoopAddress;
				break;
			}
		}
   
		if(j==instrument_map_index)
		{
			pAdpcmSampleData = pbSampleData + ((LoopAddress-pHander->wStartOffset)>>1);
			sampleLen = (UINT32)pHander->wEndOffset + pHander->wStartOffset;
			if(sampleLen%8!=0) 	sampleLen = (( ( sampleLen>>3) +1 )<<3);

			state.index=0;
			state.valprev=0;
			ima_adpcm_decode_multisamples(pPcmSampleData, (char *)pAdpcmSampleData, sampleLen>>1,&state);

			instrument_map[j].OldLoopAddress=LoopAddress;
			LoopAddress=NewOffset+pHander->wStartOffset;//sample offset
			instrument_map[j].NewLoopAddress=LoopAddress;
			NewOffset+=sampleLen;
			pPcmSampleData+=sampleLen;
			instrument_map_index++;			
		}

		pHander->dLoopAddress=LoopAddress;

	}

	me_free(instrument_map);
	return ME_SUCCESS;
}

static int Vcnt_Get_Sample(UINT8 bBankID, UINT8 bTimbre, UINT8 bNote,Download_Zone * pHander)
{
	UINT8		bMatchNum = 0;
	UINT8		bSampleIdx;
	UINT8		bBankIdx;
	UINT16		wFirSample;
	UINT8		bSampleNum;
	UINT8		*pCurByte;
	UINT8		*pByte;	
	UINT8   	*pBeginByte;
	UINT8		*pEndByte;
	UINT16	    wPresetNum;
	UINT8   	bFind;
	UINT8		*pBank = (PUINT8)BankHeaderArray;

    wPresetNum = (*(pBank + 8));

	if (bBankID == 128) 
	{
		if((bNote<35)||(bNote>81))	//GM level 1 only.
			return 0;

		pCurByte = (UINT8*)(pBank + 18 + 4*bBankID);
	}
	else if (bBankID == 0) 
	{
    		pCurByte = (UINT8*)(pBank + 18 + 4*bTimbre);
	}
    else if(bBankID == 1)        // china instrument
    {
		pCurByte = (UINT8*)(pBank + 18 + 4*(bTimbre+129));
	}
	 else
	 {
        //fengyuhong modify
		//bBankID = bBankID;
        return 0; //error
	 }
	
	bBankIdx = pCurByte[0];
	bSampleNum = pCurByte[1];
	wFirSample = ((UINT16)pCurByte[3]<<8) + (UINT16)pCurByte[2];

	if ((bBankIdx != bBankID) || bSampleNum == 0)//not macth bank number or empty preset
	{
    		return 0;
	}

	pBeginByte = (UINT8*)(pBank + 18 + wPresetNum*4 + PARALEN*wFirSample);
	pEndByte = (UINT8*)(pBank + 18 + wPresetNum*4 + PARALEN*(wFirSample + bSampleNum - 1));
	pCurByte = pBeginByte;
	bFind = 0;
	for(bSampleIdx = 0 ; bSampleIdx < bSampleNum ; bSampleIdx++)
	{
		if(pCurByte[0] <= bNote && pCurByte[1] >= bNote)
		{
			bFind = 1;
			pByte = pCurByte;
		}
		

		if((bFind == 1))
		{
			pHander->bBankID=bBankID;
			pHander->bTimbre=bTimbre;
			pHander->bLowNote 		=  pByte[0];
			pHander->bHighNote 		=  pByte[1];
			pHander->bSamplePitch 	=  pByte[2];
			pHander->sbPitchCorrect 	=  pByte[3];
			pHander->wStartOffset 		=  (UINT16)((UINT16)pByte[4] + ((((UINT16)pByte[5])<<8)&0xFF00));
			pHander->wEndOffset 		=  (UINT16)((UINT16)pByte[6] + ((((UINT16)pByte[7])<<8)&0xFF00));
			pHander->dLoopAddress 	=  (UINT32)((UINT32)pByte[8]+((((UINT32)pByte[9]<<8))&0xFF00)+((((UINT32)pByte[10])<<16)&0xFF0000)+((((UINT32)pByte[11])<<24)&0xFF000000));

/*			if(BankDataType==1)
               pHander->dLoopAddress=Mapping_adpcm(pHander); 
            else
			   pHander->dLoopAddress=Decoder_adpcm(pHander);*/

			pHander->wSampleRate 	=  (UINT16)((UINT16)pByte[12]+(((UINT16)(pByte[13]<<8))&0xFF00));
			pHander->bACounter 		=  (UINT16)((UINT16)pByte[14] + ((((UINT16)pByte[15])<<8)&0xFF00));
			pHander->bDCounter 		=  (UINT16)((UINT16)pByte[16] + ((((UINT16)pByte[17])<<8)&0xFF00));
			pHander->bRCounter 		=  (UINT16)((UINT16)pByte[18] + ((((UINT16)pByte[19])<<8)&0xFF00));
			pHander->wSLevel 		=  (UINT16)((UINT16)pByte[20]+(((UINT16)pByte[21]<<8)&0xFF00));
			pHander->wAttenuation 	=  (UINT16)((UINT16)pByte[22]+(((UINT16)pByte[23]<<8)&0xFF00));
			pHander->bBoolCtrl 		=  pByte[24];
			pHander->bLfoCounter 		=  pByte[25];                       //lfofreq
			bMatchNum++;
			break;
		
		}
		
		pCurByte += PARALEN;
	}

	if((bBankID != 128)&&(bMatchNum==0))
	{
	    if ((bNote < pBeginByte[0]) && (bNote >= 10))
	    {
			pByte = pBeginByte;
			bFind = 1;
	    }
	    else if ( (bNote > pEndByte[1]) && (bNote <= 120)) 
	    {
			pByte = pEndByte;
			bFind = 1;
	    }

		if(bFind)
		{
			pHander->bBankID=bBankID;
			pHander->bTimbre=bTimbre;
			
			pHander->bLowNote 		=  pByte[0];
			pHander->bHighNote 		=  pByte[1];
			pHander->bSamplePitch 	=  pByte[2];
			pHander->sbPitchCorrect 	=  pByte[3];
			pHander->wStartOffset 		=  (UINT16)((UINT16)pByte[4] + ((((UINT16)pByte[5])<<8)&0xFF00));
			pHander->wEndOffset 		=  (UINT16)((UINT16)pByte[6] + ((((UINT16)pByte[7])<<8)&0xFF00));
			pHander->dLoopAddress 	=  (UINT32)((UINT32)pByte[8]+((((UINT32)pByte[9]<<8))&0xFF00)+((((UINT32)pByte[10])<<16)&0xFF0000)+((((UINT32)pByte[11])<<24)&0xFF000000));
			
	/*		if(BankDataType==1)
				pHander->dLoopAddress=Mapping_adpcm(pHander); 
            else
				pHander->dLoopAddress=Decoder_adpcm(pHander);*/
			
			pHander->wSampleRate 	=  (UINT16)((UINT16)pByte[12]+(((UINT16)(pByte[13]<<8))&0xFF00));
			pHander->bACounter 		=  (UINT16)((UINT16)pByte[14] + ((((UINT16)pByte[15])<<8)&0xFF00));
			pHander->bDCounter 		=  (UINT16)((UINT16)pByte[16] + ((((UINT16)pByte[17])<<8)&0xFF00));
			pHander->bRCounter 		=  (UINT16)((UINT16)pByte[18] + ((((UINT16)pByte[19])<<8)&0xFF00));
			pHander->wSLevel 		=  (UINT16)((UINT16)pByte[20]+(((UINT16)pByte[21]<<8)&0xFF00));
			pHander->wAttenuation 	=  (UINT16)((UINT16)pByte[22]+(((UINT16)pByte[23]<<8)&0xFF00));
			pHander->bBoolCtrl 		=  pByte[24];
			pHander->bLfoCounter 		=  pByte[25];                       //lfofreq
			bMatchNum++;

			if(bNote<pHander->bLowNote)
			{
				pHander->bLowNote=0; //resize;
			}
			else if(bNote>pHander->bHighNote)
			{
				pHander->bHighNote=127; //resize;

			}
		}
	}

	return bMatchNum;
}

/***************************************************************
Description:
		Seek the synthesizing parameters of a note from default bank header.
				
Parameters:
		bBankID:		The index of the bank.
		bTimbre:		The timbre of this note
		bNote:		Note.
		pPara:		Pointer to the structure contain the seek result.
		
Returns:
		The number of seek results.
		
****************************************************************/
ME_U32 parser_mid_get_bandk_header(UINT8 bBankID, UINT8 bTimbre, UINT8 bNote,void* pvm_input)
{
	VM_MIDI_INPUT *vm_input=(VM_MIDI_INPUT *)pvm_input;
    UINT32 Zone_Len=vm_input->ZoneLen;
	Download_Zone* Down_Zone=(Download_Zone*)vm_input->pBankheader;
	Download_Zone* pHander;
	//int zone_index=-1;
	int i;
	
	for(i=0;i<Zone_Len;i++)
	{
		pHander=&(Down_Zone[i]);
		if(bBankID==pHander->bBankID)
		{
			if(bTimbre==pHander->bTimbre)
			{
				if((bNote>=pHander->bLowNote)&&(bNote<=pHander->bHighNote))
				{
					//zone_index=i;
					break;
				}
			}
		}
	}

	//if(zone_index==-1)
	if(i==Zone_Len)
	{
		if(Vcnt_Get_Sample(bBankID, bTimbre,bNote,&Down_Zone[Zone_Len]))
		{
			Zone_Len++;
			if(Zone_Len>MAX_ZONE_LEN)
			{
				ME_ERROR(("Down_Zone overflow\n"));
				Zone_Len=0;	
			}
			vm_input->ZoneLen=Zone_Len;
		}
	}

	return ME_SUCCESS;
}

ME_U32 parser_mid_get_bank_data(VM_MIDI_PARSER_INFO *file)
{
	VM_MIDI_INPUT *vm_input = (VM_MIDI_INPUT *)&(file->vm_input);
	VM_MIDI_INPUT* vm_input_core = (VM_MIDI_INPUT *)&(file->vm_input_core);
	Download_Zone* Down_Zone=(Download_Zone*)vm_input->pBankheader;
	Download_Zone* pHander;
	UINT32 Zone_Len=vm_input->ZoneLen;
	UINT32 BankDataLen;
	UINT32 BankDataType;
	UINT32 sampleLen,totalsamplelen;
	UINT32 wavenum;
	UINT32* loopaddress;
	UINT32 i,j;
	void *pBankData;
	void *pBankData_core;
	void *pAdpcmMapping;
	void *pAdpcmMapping_core;	
	ME_U32 retvalue;
	UINT32 LoopAddress;

	//calculate bank data len(byte)
	loopaddress=me_malloc(Zone_Len*sizeof(UINT32));
	if(loopaddress==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	totalsamplelen=0;
	wavenum=0;
	for(i=0;i<Zone_Len;i++)
	{
		pHander=&(Down_Zone[i]);
		LoopAddress=pHander->dLoopAddress;
		for(j=0;j<wavenum;j++)
		{
			if(LoopAddress==loopaddress[j])
				break;
		}
		if(j==wavenum)
		{
				sampleLen = (UINT32)pHander->wEndOffset + pHander->wStartOffset;
				if(sampleLen%8!=0) 	sampleLen = (( ( sampleLen>>3) +1 )<<3);
				totalsamplelen+=sampleLen; //sample number
				loopaddress[j]=LoopAddress;
				wavenum++;
		}
	}
	me_free(loopaddress);



	//malloc pcm sample buffer
	BankDataType=0;//pcm sample
	BankDataLen=totalsamplelen<<1;
	if(BankDataLen > MAX_PCM_SIZE)
	{
		goto adpcm_proc_core;
	}
	pBankData_core = (void*)MMD_SYS_Malloc(BankDataLen + 256);
	//pBankData=NULL;
	if(pBankData_core==NULL)
	{
	adpcm_proc_core:
		BankDataType=1; //adpcm sample
		BankDataLen=totalsamplelen>>1;
		pBankData_core = (void*)MMD_SYS_Malloc(BankDataLen + 256);
		if(pBankData_core==NULL)
		{
			ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
			return ME_ERR_MALLOC_MEMORY_ERROR;
		}
		pAdpcmMapping_core = (void*)MMD_SYS_Malloc(wavenum*sizeof(AdpcmWave_InstrumentMap) + 256);
		if(pAdpcmMapping_core==NULL)
		{
			ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
			return ME_ERR_MALLOC_MEMORY_ERROR;			
		}
	}

	vm_input_core->BankDataType=BankDataType;
	vm_input_core->BankDataLen=BankDataLen;
	vm_input_core->pBankData=pBankData_core;
	vm_input_core->total_len += 3 * sizeof(ME_U32); // BankData
	

	//malloc pcm sample buffer
	BankDataType=0;//pcm sample
	BankDataLen=totalsamplelen<<1;
	if(BankDataLen > MAX_PCM_SIZE)
	{
		goto adpcm_proc;
	}	
	pBankData=me_malloc(BankDataLen);
	//pBankData=NULL;
	if(pBankData==NULL)
	{
	adpcm_proc:
		BankDataType=1; //adpcm sample
		BankDataLen=totalsamplelen>>1;
		pBankData=me_malloc(BankDataLen);
		if(pBankData==NULL)
		{
			ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
			return ME_ERR_MALLOC_MEMORY_ERROR;
		}
		pAdpcmMapping=me_malloc(wavenum*sizeof(AdpcmWave_InstrumentMap));
		if(pAdpcmMapping==NULL)
		{
			ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
			return ME_ERR_MALLOC_MEMORY_ERROR;			
		}
	}

	vm_input->BankDataType=BankDataType;
	vm_input->BankDataLen=BankDataLen;
	vm_input->pBankData=pBankData;

	vm_input->total_len += 2 * sizeof(ME_U32) + BankDataLen; // BankData


	
	if(BankDataType==0)
	{//pcm sample
		vm_input->MaxSythVoice=32; // 64
		vm_input->SampleRate=48000;	
		retvalue=Decoder_adpcm(vm_input,wavenum);
		ME_DEBUG(("midi use pcm sample to syth"));		
	}
	else
	{//adpcm sample
	    //48k 16 voices or 24k 24 voices
		vm_input->MaxSythVoice=16;//24
		vm_input->SampleRate=24000;//24000
		vm_input->pAdpcmMapping=pAdpcmMapping;
		vm_input_core->pAdpcmMapping = pAdpcmMapping_core;
		vm_input->MappingLen=wavenum;
		retvalue=Mapping_adpcm(vm_input,wavenum);
		ME_DEBUG(("midi use adpcm sample to syth"));
	}
	
	vm_input_core->MaxSythVoice = vm_input->MaxSythVoice;
	vm_input_core->SampleRate = vm_input->SampleRate;	
	vm_input_core->MappingLen = vm_input->MappingLen;
/*	
	Vm_WrSram((unsigned int)(vm_input_core->pBankData), (char*)(vm_input->pBankData), BankDataLen);
	if(BankDataType == 1)
	{
		Vm_WrSram((unsigned int)(vm_input_core->pAdpcmMapping), (char*)(vm_input->pAdpcmMapping), wavenum*sizeof(AdpcmWave_InstrumentMap));
	}
*/	
	vm_input->total_len += wavenum*sizeof(AdpcmWave_InstrumentMap); // Map
	vm_input->total_len += sizeof(ME_U32);	//	Map len

	vm_input_core->total_len += 2 * sizeof(ME_U32);
	return retvalue;
}

#endif
