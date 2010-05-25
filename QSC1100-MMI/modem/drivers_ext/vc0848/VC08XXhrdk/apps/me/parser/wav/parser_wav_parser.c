#include "parser_wav_parser.h"

#if ME_KM_PARSER_WAV_EN

ME_VOID Parser_RIFF_WAVE_Chunk(ME_PS08 buffer,WAV_RIFF_WAVE_chunk *RIFF_WAVE_chunk)
{
	ME_PS08 readPtr = buffer;
	
	me_memcpy((ME_PS08)RIFF_WAVE_chunk->ID,readPtr,4);
	readPtr += 4;
	RIFF_WAVE_chunk->FileLength = *(ME_PU32)readPtr;
	readPtr += 4;
	me_memcpy((ME_PS08)RIFF_WAVE_chunk->TYPE,readPtr,4);
}

ME_VOID Parser_FORMAT_Chunk(ME_PS08 buffer,WAV_format_chunk *format_chunk)
{
	ME_PS08 readPtr = buffer;

	me_memcpy((ME_PS08)format_chunk->ID,readPtr,4);
	format_chunk->Size = *(ME_PU32)(readPtr+4);
	format_chunk->FormatTag = *(ME_PU16)(readPtr+8);
	format_chunk->Channels = *(ME_PU16)(readPtr+10);
	format_chunk->SampleRate = *(ME_PU32)(readPtr+12);
	format_chunk->AvgBytesPerSec = *(ME_PU32)(readPtr+16);
	format_chunk->BlockAlign = *(ME_PU16)(readPtr+20);
	format_chunk->BitPerSample = *(ME_PU16)(readPtr+22);
	//fengyuhong add for ima-adpcm
	if(format_chunk->FormatTag==WAVE_FORMAT_DVI_ADPCM)
	{
		format_chunk->cbSize=*(ME_PU16)(readPtr+24);
		format_chunk->nSamplesPerBlock=*(ME_PU16)(readPtr+26);
	}
}

//fengyuhong add for ima-adpcm 
ME_VOID Parser_FACT_Chunk(ME_PS08 buffer,WAV_fact_chunk *fact_chunk)
{
	ME_PS08 readPtr = buffer;
	
	me_memcpy((ME_PS08)fact_chunk->ID,readPtr,4);
	fact_chunk->Size = *(ME_PU32)(readPtr+4);
	fact_chunk->Data = *(ME_PU32)(readPtr+8);
}

ME_VOID Parser_DATA_Chunk(ME_PS08 buffer,WAV_Data_chunk *data_chunk)
{
	ME_PS08 readPtr = buffer;

	me_memcpy((ME_PS08)data_chunk->ID,readPtr,4);
	data_chunk->Size = *(ME_PU32)(readPtr+4);
}

ME_U08 Parser_wave_head(wav_infor_parser *file,WAV_format_chunk *format_chunk,WAV_Data_chunk *data_chunk,WAV_fact_chunk *fact_chunk)
{
	ME_S32 have_parsered_length=0;
	ME_PS08 readBuf;
	ME_S32 bytesLeft;
	ME_S08 cmp_data[4]={0x64,0x61,0x74,0x61};	//"data"
	int i;
	
	readBuf = (ME_PS08)me_malloc(32);
	have_parsered_length = sizeof(WAV_RIFF_WAVE_chunk);		//length of RIFF WAVE chunk

	//Parser format chunk of header
	file->me_seek(file->stream,have_parsered_length,SEEK_SET);
	bytesLeft=file->me_read(file->stream,readBuf,sizeof(WAV_format_chunk));
	if(bytesLeft != sizeof(WAV_format_chunk))
	{
		me_free(readBuf);
		return ME_FAILE;
	}
	Parser_FORMAT_Chunk(readBuf,format_chunk);
	have_parsered_length+=(format_chunk->Size+8);
	
	//fengyuhong add for IMA-adpcm parse
    if(format_chunk->FormatTag==WAVE_FORMAT_DVI_ADPCM) //IMA-ADPCM foramt
	{
		file->me_seek(file->stream,have_parsered_length,SEEK_SET);
		bytesLeft=file->me_read(file->stream,readBuf,sizeof(WAV_fact_chunk));
		if(bytesLeft != sizeof(WAV_fact_chunk))
		{
			me_free(readBuf);
			return ME_FAILE;
		}
        
		Parser_FACT_Chunk(readBuf,fact_chunk);
		have_parsered_length+=(fact_chunk->Size+8);
	}
	
	//Parser data chunk of header
	while(1)
	{
		file->me_seek(file->stream,have_parsered_length,SEEK_SET);
		bytesLeft=file->me_read(file->stream,readBuf,8);
		if(bytesLeft != 8)
		{
			me_free(readBuf);
			return ME_FAILE;
		}
		Parser_DATA_Chunk(readBuf,data_chunk);
		for(i=0;i<4;i++)
		{
			if(data_chunk->ID[i]!=cmp_data[i])
			{
				have_parsered_length+=(data_chunk->Size+8);
				break;
			}
		}
		if(i==4)
		{//The chunk is data chunk
			have_parsered_length+=8;
			file->me_seek(file->stream,have_parsered_length,SEEK_SET);
			break;
		}
	}
	me_free(readBuf);
	return ME_SUCCESS;
}
#endif
