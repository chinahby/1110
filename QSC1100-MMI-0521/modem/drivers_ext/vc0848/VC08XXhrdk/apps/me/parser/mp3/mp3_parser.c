/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	mp3_parser.h
* User for : 	820
* Creat By :   Zhang Xuecheng()                                                                   
* CrateDate: 	2007/09/20
* Last modify : Zhang Xuecheng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Zhang Xuecheng
=======================================================================*/
#include "mp3_parser.h"
#include "..\..\id3\mp3_id3.h"
#include "..\..\..\..\inc\mmd_file_driver.h"
#include "..\..\..\media\media_utils.h"
#include "mp3_commontabs.h"

//#define SRC_FRAME_DEBUG
//: Add by antonliu, 2008/06/12, for debug
#ifdef SRC_FRAME_DEBUG
#define TMP_BUF_NUM			16
#define TMP_MAX_FRAME			0

static unsigned int tmp_count = 0;
unsigned char* tmp_bufs;
#endif
///:~
unsigned int g_backup_syncword = 0;
int CheckSyncWord_single(unsigned int headWord);
//: Add by antonliu, 2008/06/10
static const unsigned short bitrate_table[5][16] = {
		{0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0xffff},
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,0xffff},
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320, 0xffff},
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0xffff},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0xffff}
};

static const long samplerate_table[3][4] = { 
		{44100, 48000, 32000, 0},
		{22050, 24000, 16000, 0},
		{11025, 12000,  8000, 0}
};
///:~

static const short tabsel_123 [2] [3] [16] = {
	{ {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
	  {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
	  {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },
	
	{ {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
	  {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
	  {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

static const long freqs[9] = { 44100, 48000, 32000,
			       22050, 24000, 16000,
			       11025, 12000,  8000 };

MPEGAudioFrameInfo Frame;
int frm_count = 0;
mp3_infor_t* mp3_init(void)
{
	mp3_infor_t *mp3_file = MMD_Malloc(sizeof(mp3_infor_t));
	frm_count = 0;
	g_backup_syncword = 0;
	if(mp3_file == NULL)
		return 0;
	
	me_memset((ME_PU08)mp3_file, 0, sizeof(mp3_infor_t));

	mp3_file->mp3DecInfo = (MP3DecInfo*)(MMD_Malloc(sizeof(MP3DecInfo)));
	if(mp3_file->mp3DecInfo == NULL)
		return 0;
	
	me_memset((ME_PU08)(mp3_file->mp3DecInfo), 0, sizeof(MP3DecInfo));
	
#ifdef SRC_FRAME_DEBUG
	tmp_bufs = MMD_Malloc(TMP_BUF_NUM * 1024);
	tmp_count = 0;
#endif
	return mp3_file;

}
ME_U32 mp3_close(mp3_infor_t *file)
{
	MMD_Free(file->mp3DecInfo);
	MMD_Free(file);
	return 0;
}

ME_U32 mp3_parser_read_buffer(void *buffer, int count, mp3_infor_t * file)
{
	file->seek_func(file->stream, (file->uloffset), SEEK_SET);
	return file->read_func(file->stream, buffer, count);
}

//: Add by antonliu, 2008/06/03
unsigned int inverse_byte(unsigned char* src)
{
	unsigned char ch[4];
	unsigned int result = 0;
	unsigned int i;
	for(i=0; i<4; i++)
	{
		ch[i] = src[i];
	}
	result = (ch[0] << 24) | (ch[1] << 16) |  (ch[2] << 8) | ch[3];
	return result;
}

unsigned char g_free_bitrate_flg = 0;

ME_U32 get_mp3_headinfo(MPEGAudioHeaderInfo* pHeaderInfo, unsigned char* stream_buffer, int* buflen)
{
	unsigned int mp3_header;
	
	unsigned short mp3_sync;
	unsigned char mpeg_version;
	unsigned char mpeg_layer;
	unsigned char protect_bit;
	unsigned char bitrate_idx;
	unsigned char samprate_idx;	
	unsigned char padding;	
	unsigned char channel_mode;
	unsigned char vl_idx = 0;
	unsigned char v_idx = 0;
	unsigned int search_count = 0;
	unsigned int bytes_left = *buflen;
head_info_err_ret:	
	if(search_syncword(stream_buffer+search_count, buflen) != 0)
	{
		//modify by yjz 2009/3/6
		if(search_count == 0)
		{
			*buflen = MAX_BUF_LENGTH;
		}
		else
		{
			*buflen = search_count;   
		}		
		return 1;
	}
	else
	{
		mp3_header = inverse_byte(stream_buffer+search_count+(*buflen));
	}


	if(CheckSyncWord_single(mp3_header) != 1)
	{
		search_count += (*buflen + 1);
		bytes_left -= (*buflen + 1);
		*buflen = bytes_left;
		goto head_info_err_ret;
	}
	*buflen += search_count;
	
	mp3_sync = (mp3_header >> MP3_FRAME_SYNC) & 0x7ff;
	mpeg_version = (mp3_header >> MP3_AUDIO_VERSION) & 0x3;
	mpeg_layer = (mp3_header >> MP3_LAYER) & 0x3;
	protect_bit = (mp3_header >> MP3_PROTECT_BIT) & 0x1;
	bitrate_idx = (mp3_header >> MP3_BITRATE_IDX) & 0xf;
	samprate_idx = (mp3_header >> MP3_SAMPLERATE_IDX) & 0x3;	
	padding = (mp3_header >> MP3_PADDING_BIT) & 0x1;	
	channel_mode = (mp3_header >> MP3_CHANNEL_MODE) & 0x3;

	if(mp3_sync != 0x7ff)
	{
		goto head_info_err_ret;
	}
	if(mpeg_version == 1)
	{
		goto head_info_err_ret;
	}
	else
	{
		if(mpeg_version == VERSION_MPEG_2_5)
		{
				pHeaderInfo->nVersion = 0;
				v_idx = 2;
		}
		else if(mpeg_version == VERSION_MPEG_2)
		{
				pHeaderInfo->nVersion = 2;
				v_idx = 1;
		}
		else if(mpeg_version == VERSION_MPEG_1)
		{
				pHeaderInfo->nVersion = 3;
				v_idx = 0;
		}
	}
	
	switch(mpeg_layer)
	{
		case 0:
			goto head_info_err_ret;
		case 1:
			pHeaderInfo->nLayer = 3;
			if(mpeg_version == VERSION_MPEG_1)
			{
					vl_idx = 2;
			}	
			else if(mpeg_version == VERSION_MPEG_2 || mpeg_version == VERSION_MPEG_2_5)
			{
					vl_idx = 4;
			}
			break;
		case 2:
			pHeaderInfo->nLayer = 2;
			if(mpeg_version == VERSION_MPEG_1)
			{
					vl_idx = 1;
			}
			else if(mpeg_version == VERSION_MPEG_2 || mpeg_version == VERSION_MPEG_2_5)
			{
					vl_idx = 4;
			}
			break;
		case 3:
			pHeaderInfo->nLayer = 1;
			if(mpeg_version == VERSION_MPEG_1)
			{
					vl_idx = 0;
			}
			else if(mpeg_version == VERSION_MPEG_2 || mpeg_version == VERSION_MPEG_2_5)
			{
					vl_idx = 3;
			}
			break;
		default:
			break;
	}

	if(bitrate_idx == 0xf)
	{
		goto head_info_err_ret;
	}
	else
	{
		pHeaderInfo->nBitRate = bitrate_table[vl_idx][bitrate_idx];
	}
	

	if(samprate_idx == 3)
	{
		goto head_info_err_ret;
	}	
	else
	{
		pHeaderInfo->nSampleRate = samplerate_table[v_idx][samprate_idx];
	}		

	if(channel_mode == CHN_MODE_MONO)
	{
		pHeaderInfo->nChannels = 1;
	}
	else
	{

		pHeaderInfo->nChannels = 2;
	}
	
	pHeaderInfo->nFarmeSize = compute_frame_size((int)mpeg_version, (int)mpeg_layer, (int)bitrate_idx, (int)samprate_idx, (int)padding);

	if(g_backup_syncword == 0)
	{
		g_backup_syncword = mp3_header;
	}
	return ERR_OK;

}

/**************************************************************************************
* Function:		CheckSyncWord
*
* Description:	decide whether current syncword is legal by  checking the 
                content of related headword.                
*
* Inputs:	  the headword related to current syncword(unsigned int headWord) 
*
* Outputs:      
*
* Return:		check result: 1-ok, 0-illegal header&false syncword
*
* Notes:        strict checking here :mp3 streams generated by some encoders may could
                not pass.
**************************************************************************************/
int CheckSyncWord_single(unsigned int headWord)
{

	/*first 11 bits must be '1'*/
	if((headWord & SYNCH_MASK) != SYNCH_MASK)
		return 0;

	/*reserved  versio id ?*/
	if((headWord & MPEGID_MASK) == 0x00080000)
		return 0;
	
	/*reserved layer?*/
	if((headWord & LAYER_MASK) == 0x0)
		return 0;
	
	/*bad bitrate idx?*/
	if((headWord & BITRATE_IDX_MASK) == BITRATE_IDX_MASK)
		return 0;
	
	/*reserved samplerate idx ?*/
	if((headWord & SAMPLERATE_IDX_MASK) == SAMPLERATE_IDX_MASK)
		return 0;
	

	/*MPEG2.5 only layer 3*/
	if(((headWord & MPEGID_MASK) == 0x0) && ((headWord&LAYER_MASK)!=0x00020000))
		return 0;	

	if(g_backup_syncword != 0)
	{
		if( (headWord & MPEGID_MASK) != (g_backup_syncword & MPEGID_MASK)
		||(headWord & LAYER_MASK) != (g_backup_syncword & LAYER_MASK)
		||(headWord & SAMPLERATE_IDX_MASK) != (g_backup_syncword & SAMPLERATE_IDX_MASK) )
		{
			return 0;
		}
	}
	return 1;
    
}

ME_U32 search_syncword(unsigned char* buf, int* buf_len)
{
	int i = 0;
	for (i = 0; i <= *buf_len - 4; i++) {
		if ( (buf[i] & 0xff) == 0xff && (buf[i+1] & 0xe0) == 0xe0 )
		{
			*buf_len = i;
			return 0;
		}
	}
	return -1;
}

static void resetSynchState(MP3DecInfo *PSmp3DecInfo)
{
	int i;
	
	SynchInfo* synch_info = (SynchInfo*) (PSmp3DecInfo->SyncBuf);
    
    for (i = 0; i < MAX_SYNCHS; i++)
    {
        synch_info->synchs[i].active = 0;
		synch_info->synchs[i].paired_flag = 0;
    }

    synch_info->offset_current = 0;
    synch_info->max_synch_index = 0;

}
/**************************************************************************************
* Function:		compute_frame_size
*
* Description:	calculates size (in bytes) of MPEG frame             
*
* Inputs:		2-bit ID index(int mpegid) 		
*				2-bit layer index(int layer)
*				4-bit bitrate index(int bitrate)
*				2-bit sample-rate index(int sampfreq)	
*				1-bit padding flag(int padding)
* Outputs:      NONE. 
*
* Return:		number of bytes in frame
* Notes:        index mapping should be checked carefully.
**************************************************************************************/
unsigned int compute_frame_size(	int mpegid , 
									int layer,
									int bitrate, 
									int sampfreq, 
									int padding 
								)
{
    unsigned int br, k0;                        /* misc. scratch variables            */
    unsigned int bytesPerSlot;                  /* depends on if Layer I or not       */
	
    unsigned int sr;
	
    unsigned int nSlots;
	
	/*map mpeg ids onto correct table indices*/
	mpegid = vesionIDTab[mpegid]; 

	if (layer == LAYER_1)
        bytesPerSlot = BYTES_PER_SLOT_L1;
    else
        bytesPerSlot = BYTES_PER_SLOT_L23;
	
    /*map layer*/
	layer = 3 - layer;

	sr = (unsigned int)samplerateTab[mpegid][sampfreq];
	br = (unsigned int)bitrateTab[mpegid][layer][bitrate];
    k0 = (unsigned int)k_nSlots[mpegid][layer];
	
    nSlots = (k0*br*1000 /sr);					/*should not overflow?*/

    if (padding)
        nSlots++;
	

    return (bytesPerSlot * nSlots);
}

/**************************************************************************************
 * Function:    MP3FindSyncWord
 *
 * Description: locate the next byte-aligned sync word in the raw mp3 stream
 *
 * Inputs:      buffer to search for sync word
 *              max number of bytes to search in buffer
 *
 * Outputs:     none
 *
 * Return:      offset to first sync word (bytes from start of buf)
 *              -1 if sync not found after searching nBytes
**************************************************************************************/
int MP3FindSyncWord(unsigned char *buf, int nBytes)
{
	int i;
	
	/* find byte-aligned syncword - need 12 (MPEG 1,2) or 11 (MPEG 2.5) matching bits */
	for (i = 0; i <= nBytes - 4; i++) {
		if ( (buf[i] & SYNCWORDH) == SYNCWORDH && (buf[i+1] & SYNCWORDL) == SYNCWORDL )
			return i;
	}
	
	return -1;
}

/**************************************************************************************
* Function:		CheckSyncWord
*
* Description:	decide whether current syncword is legal by  checking the 
                content of related headword.                
*
* Inputs:		pointer to new syncinfo struct(Synch* new_synch) 		
*				the headword related to current syncword(unsigned int headWord) 
*
* Outputs:      updated new_synch struct.
*
* Return:		check result: 1-ok, 0-illegal header&false syncword
*
* Notes:        strict checking here :mp3 streams generated by some encoders may could
                not pass.
**************************************************************************************/
int CheckSyncWord(Synch* new_synch, unsigned int headWord)
{

	/*first 11 bits must be '1'*/
	if((headWord & SYNCH_MASK) != SYNCH_MASK)
		return 0;

	/*reserved  versio id ?*/
	if((headWord & MPEGID_MASK) == 0x00080000)
		return 0;
	
	/*reserved layer?*/
	if((headWord & LAYER_MASK) == 0x0)
		return 0;
	
	/*bad bitrate idx?*/
	if((headWord & BITRATE_IDX_MASK) == BITRATE_IDX_MASK)
		return 0;
	
	/*reserved samplerate idx ?*/
	if((headWord & SAMPLERATE_IDX_MASK) == SAMPLERATE_IDX_MASK)
		return 0;
	
	
	
	/*For MPEG-1  Layer II there are some combinations of bitrate and stereo mode
	  which are not allowed, some encoders ignore this restriction*/
#if 1 
	if( ((headWord & MPEGID_MASK)== MPEGID_MASK) && ((headWord & LAYER_MASK)==0x00040000) )
	{
		int stereo_mode  =	(headWord&STEREO_MODE_MASK)>>6;
		int bitrate_indx =	(headWord&BITRATE_IDX_MASK)>>12;
		
		if(((unsigned int)(abl2[bitrate_indx])) & (1 << stereo_mode)) 
			return 0;
	} 
#endif
	
	
	/*MPEG2.5 only layer 3*/
	if(((headWord & MPEGID_MASK) == 0x0) && ((headWord&LAYER_MASK)!=0x00020000))
		return 0;	
	
#ifdef	OPT_STRICT
	/*
	* ISO/IEC 11172-3 says this is a reserved emphasis value, but
	* streams exist which use it anyway. Since the value is not important
	* to the decoder, we allow it unless OPT_STRICT is defined.
	*/
	if((headWord&EMPHASIS_MASK)== EMPHASIS_RES_VAL)
		return 0;
#endif//OPT_STRICT

	new_synch->header	= headWord;
    new_synch->active	= 1;
	
	
    {
        int mpegid   = (headWord & MPEGID_MASK) >> 19;
        int layer    = (headWord & LAYER_MASK)>>17;
        int bitrate  = (headWord & BITRATE_IDX_MASK) >> 12;
        int sampfreq = (headWord & SAMPLERATE_IDX_MASK) >> 10;
        int padding  = (headWord & PADDING_MASK) >> 9;
		
		
        if (bitrate > 0)
        {
            new_synch->next_header_distance = compute_frame_size(mpegid,layer,bitrate,sampfreq,padding);
			new_synch->bitrate_idx_zero_flag = 0;
        }
        else
        {
            new_synch->next_header_distance = 0;
			new_synch->bitrate_idx_zero_flag = 1;
        }
		
		if(layer == LAYER_1)	new_synch->padding = padding*BYTES_PER_SLOT_L1;
		else					new_synch->padding = padding*BYTES_PER_SLOT_L23;
		
		

		return 1;
    }

    
}

static void remove_synch(Synch* synchs, int index, SynchInfo* synch_info)
{
    synchs[index].active = 0;

    if (index == synch_info->max_synch_index-1)
    {
        synch_info->max_synch_index--;
    }
}


/**************************************************************************************
* Function:		synchs_are_compatible
*
* Description:	decide whether content of two headword struct is consistent                
*
* Inputs:		pointer to new sync struct(Synch*  new_sync) 
*               pointer to old sync struct(Synch*  old_sync)				
*				
*
* Outputs:      NONE
*
* Return:		check result: 1-yes, 0-no
*
**************************************************************************************/

static int synchs_are_compatible(Synch*  new_sync, Synch* old_sync)
{
    if ((new_sync->header ^ old_sync->header) & HEADER_MASK)
    {
        return 0;
    }
    
	if(new_sync->bitrate_idx_zero_flag != old_sync->bitrate_idx_zero_flag)	
    {
        return 0;
    }
	
	if(	(new_sync->header&STEREO_MODE_MASK == STEREO_MODE_MASK)	&&	
		(old_sync->header&STEREO_MODE_MASK != STEREO_MODE_MASK)
	  )
	{
		return 0;
	}

	if(	(new_sync->header&STEREO_MODE_MASK != STEREO_MODE_MASK)	&&	
		(old_sync->header&STEREO_MODE_MASK == STEREO_MODE_MASK)
	  )
	{
		return 0;
	}
	
	

    return 1;
}

/**************************************************************************************
* Function:		store_synch
*
* Description:	store new paired sync info. 
*
* Inputs:		pointer to new sync struct(Synch*  new_sync) 
*               pointer to old sync struct(Synch* synch_slot)	
*			    pointer to synch info struct(SynchInfo* synch_info) 
*				pointer to empty slot no(int* pempty_slot)
*				the number of additional successive synchs required(int synchs_required)
*
* Outputs:      NONE
*
* Return:		check result: 1-yes, 0-no
*
* NOTE:			For back trace, old sync will not be replaced by new sync.
                If missing of some first frames is ok, old sync could be replaced by new
				sync for saving memory.
**************************************************************************************/
static int store_synch(Synch* new_synch, Synch* synch_slot,  SynchInfo* synch_info, int* pempty_slot, int synchs_required)
{
    int empty_slot = *pempty_slot;
	Synch* synchs = synch_info->synchs;
				
	new_synch->count = synch_slot->count + 1;
  
	if (new_synch->count == synchs_required)
    {
        return 1;
    }

    if (new_synch->bitrate_idx_zero_flag == 1)
    {
        /*free format mode*/
		new_synch->next_header_distance  =	new_synch->offset - synch_slot->offset - synch_slot->padding + new_synch->padding;	
		
		if(synch_slot->next_header_distance==0){
			synch_slot->next_header_distance =	new_synch->offset - synch_slot->offset;
			synch_slot->next_synch_offset = new_synch->offset;
		}
     	
		new_synch->next_synch_offset =	new_synch->offset + new_synch->next_header_distance;

    }
    else
    {
        /*not free format mode*/
		new_synch->next_synch_offset	=  new_synch->offset + new_synch->next_header_distance;	
		
    }

	/*store this new sync*/
	if(empty_slot>0) 	synch_info->synchs[empty_slot] = *new_synch;
	else
	{
	
		for(empty_slot=0; empty_slot<synch_info->max_synch_index; empty_slot++)
		{
			if(synchs[empty_slot].active ==0) break;
		}

		if(empty_slot>=MAX_SYNCHS) return 0;
		
		*pempty_slot = empty_slot;
		
		synch_info->synchs[empty_slot] = *new_synch;
	}

	
	return 0;
}

static void store_new_synch(Synch*  new_synch, SynchInfo* synch_info, int index, int offset_current)
{
    if (new_synch->bitrate_idx_zero_flag)
    {
		new_synch->count = (char)(-1);
		new_synch->next_synch_offset = -1;		/*unknown here*/
		
    }
    else
    {
        new_synch->next_synch_offset = offset_current + new_synch->next_header_distance;
		
        new_synch->count = 0;
    }

    if (index == -1)
    {
        if (synch_info->max_synch_index >= MAX_SYNCHS)
        {
            return;
        }
        else
        {
            index = synch_info->max_synch_index;
			synch_info->max_synch_index++;
        }
    }
	
    synch_info->synchs[index] = *new_synch;
	synch_info->max_synch_index = MAX(synch_info->max_synch_index,index+1);
}

/**************************************************************************************
* Function:		MP3DecFindSyncWord
*
* Description:	This function searches all the valid data in the bit stream buffer for two or more
*				successive valid synchronization words, at the correct distance of each other
*				according to the header information. In case of free-format bit rate, it searches for
*				two consecutive equal and valid distances between synch words with valid
*				headers indicating free-format bit-rates
*
* Inputs:		mp3 decoder handle instance(HMP3Decoder hMP3Decoder)
				buffer to search for sync word(unsigned char *buf)
*				max number of bytes to search in buffer(int nBytes)
*				the number of additional successive synchs required, a value of 1 looks for two 
*               successive synchs(int synchs_required).
*
* Outputs:		offset to first certified sync word (bytes from start of buf)	
*               -1 if sync not found after searching nBytes. 
*
* Return:		mp3dec_ERR_OK if sync successfully,	
*				mp3dec_ERR_INDATA_UNDERFLOW if at least one legal sync is found but could not find enough paired ones.
*               mp3dec_ERR_NO_SYNC if no legal sync is found.
**************************************************************************************/
int MP3DecFindSyncWord(	void* hMP3Decoder,
								unsigned char *buf,
								int nBytes,
								int synchs_required,
								int *offset_sync
							 )
{
	
    int i;
	int offset_tmp = -1;
	int bytesLeft = nBytes;
	MP3DecInfo* mp3DecInfo = (MP3DecInfo*)hMP3Decoder;
	unsigned int HeadWordRef;		/*reference headword, make sure it's correct*/
	unsigned int HeadWordCurr;		/*the current candidate headword*/

	SynchInfo* synch_info;	
	Synch new_synch;
	
	int synch_found = 0;
	int sync_count  = 0;
	
	unsigned char *BufPtr = buf;

	if(mp3DecInfo==NULL) return mp3dec_ERR_NULL_POINTER;

	synch_info = (SynchInfo*) (mp3DecInfo->SyncBuf);

    /* check for synch state*/
    if (mp3DecInfo->state != 0)
    {
        resetSynchState(mp3DecInfo);
        mp3DecInfo->state = 0;
    }

    if(mp3DecInfo->header_set_flag)
		HeadWordRef = mp3DecInfo->header_word_bakup;
	else
		HeadWordRef = 0;

	synch_info->offset_current = 0;

	/*	search&verify process */
	do {
		/* search syncword */
		offset_tmp = MP3FindSyncWord(BufPtr, bytesLeft);
		if (offset_tmp >= 0)
		{
			BufPtr += offset_tmp;
			bytesLeft -= offset_tmp;
			synch_info->offset_current = BufPtr - buf;			/*absolute offset*/	
		}
		else if(sync_count>0)
		{
		
			/*could not find another sync before sync successfully*/
			return mp3dec_ERR_INDATA_UNDERFLOW;
		}
		else
		{
			*offset_sync = -1;
			return mp3dec_ERR_NO_SYNC;
		}
			
		
		/*read out the headword related to the sync just found*/
		HeadWordCurr = 0;
		for(i=0; i<4; i++){
			unsigned int byte_tmp = (unsigned int)BufPtr[i];
			HeadWordCurr |= (byte_tmp<<((3-i)*8));
		}

		/*searching is done byte by byte, 
		a time-consuming process*/
		BufPtr++;		
		bytesLeft--;
		
		synch_found = 0;
		/* check Possible synch */
		if (!HeadWordRef || !((HeadWordCurr ^ HeadWordRef) & HEADER_MASK))
		{
			/*checking headword content*/
			if(CheckSyncWord(&new_synch,HeadWordCurr))
			{
				 synch_found = 1;
			}
				
		}
		
		/*syntax-legal sync found*/
		if(synch_found)
		{
			/*Check if it matches a previous synch*/
			int empty_slot = -1;
			int match_found = 0;
			int max_synch_index_tmp = 0;
			
			new_synch.offset = synch_info->offset_current;
			new_synch.paired_flag = 0;
			
			/*candidate sync found */
			++sync_count;
			
			/*store first sync offset*/
			if(sync_count==1) 
				*offset_sync = synch_info->offset_current;
			
			
			for(i=0; i< synch_info->max_synch_index; i++)
			{
				Synch* synchs = synch_info->synchs;
				if (synchs[i].active)
				{
					if ((synchs[i].paired_flag==0)&&
						(synchs[i].next_synch_offset < new_synch.offset) && (synchs[i].next_synch_offset > 0)
						
						)
					{
						/*expected synch position passed, so this was not a valid synch*/
						remove_synch(synchs, i, synch_info);
						empty_slot = i;
					}

				
					else if ((synchs[i].next_synch_offset == new_synch.offset) || (synchs[i].next_synch_offset<0))
					{
						/*	position-paired sync found or previous sync is a free format one	*/

						 /*check whether header content is consistent*/
						 if (synchs_are_compatible(&new_synch, synchs+i))
						 {
							match_found = 1;												
							new_synch.paired_last_sync_idx = i;
							synchs[i].paired_flag = 1;

							if (store_synch(&new_synch, synchs+i, synch_info, &empty_slot, synchs_required))
							{
								int j ;
								int Idx;
								unsigned int header_tmp;
								Synch* sync_tmp = &new_synch;

								/*sync successfully*/		
							
								/*   back trace   */
								if(new_synch.bitrate_idx_zero_flag)
									synchs_required++;

								for(j=0; j<synchs_required; j++)
								{
									Idx = sync_tmp->paired_last_sync_idx;
									sync_tmp = synchs + Idx;
								}

								header_tmp = sync_tmp->header;
								*offset_sync = sync_tmp->offset;

								/*						update	info   					*/
								
								mp3DecInfo->freeBitrateFlag = sync_tmp->bitrate_idx_zero_flag;
									
								/*mapping is done here*/
								Idx = (header_tmp&MPEGID_MASK) >> 19;
								mp3DecInfo->version = (MPEGVersion)( Idx == 0 ? MPEG25 : ((Idx & 0x01) ? MPEG1 : MPEG2) );	
								mp3DecInfo->layer = 4 - ((header_tmp & LAYER_MASK)>>17);
								
								Idx = (StereoMode)((header_tmp&STEREO_MODE_MASK)>>6);
								mp3DecInfo->nChans = (Idx == Mono ? 1 : 2);
										
								Idx = (header_tmp & SAMPLERATE_IDX_MASK) >> 10;
								mp3DecInfo->samprate = samplerateTab[mp3DecInfo->version][Idx];
								
								mp3DecInfo->nSampsPerCh = samplesPerFrameTab[mp3DecInfo->version][mp3DecInfo->layer-1];
								
								Idx = (header_tmp & BITRATE_IDX_MASK) >> 12;
								mp3DecInfo->bitrate = ((int)bitrateTab[mp3DecInfo->version][mp3DecInfo->layer-1][Idx]) * 1000;
								
								
								mp3DecInfo->next_header_distance = sync_tmp->next_header_distance;
								if(mp3DecInfo->header_set_flag==0)
								{
									mp3DecInfo->header_set_flag = 1;
									mp3DecInfo->header_word_bakup = header_tmp;
								}

								mp3DecInfo->state = 1;
							
								if(sync_tmp->bitrate_idx_zero_flag)
								{
									int bytesPerSlot = (mp3DecInfo->layer == LAYER_1_MAP ? BYTES_PER_SLOT_L1 : BYTES_PER_SLOT_L23);
									/*remove padding */
									mp3DecInfo->freeBitrateSlots = (sync_tmp->next_header_distance - sync_tmp->padding)/bytesPerSlot;	
								}

										
								return mp3dec_ERR_OK;
							}

							max_synch_index_tmp = MAX(synch_info->max_synch_index, empty_slot+1); 
						
								
						 }
					}
						

				}
				else if (empty_slot < 0)
				{
					empty_slot = i;
				}
			
				
			}
			

			if (!match_found)
			{
				store_new_synch(&new_synch, synch_info, empty_slot, synch_info->offset_current);
			}
			else
				synch_info->max_synch_index = MAX(synch_info->max_synch_index,max_synch_index_tmp);
			
			
		}//if(synch_found) 


	} while(1);


}


ME_U32 init_Frame(MPEGAudioFrameInfo* pFrame, MPEGAudioHeaderInfo* pHeaderInfo)
{
	return 0;
}

ME_U32 get_free_bitrate(unsigned char* stream_buffer, MPEGAudioHeaderInfo* pHeaderInfo, unsigned int* framesize, unsigned int* bitrate)
{
	*framesize = 0;
	if(search_syncword(stream_buffer, (int*)framesize) != 0)
	{
		return 1;
	}
	if(search_syncword(stream_buffer+4, (int*)framesize) != 0)
	{
		return 1;
	}	
	*framesize += 4;
	if(pHeaderInfo->nVersion == 3)	//	Version 1
	{
		if(pHeaderInfo->nLayer ==  1)	//	layer 1
		{
			*bitrate = (*framesize * pHeaderInfo->nSampleRate) / (48 * 1000);
		}
		else
		{
			*bitrate = (*framesize * pHeaderInfo->nSampleRate) / (144 * 1000);
		}
	}
	else		//	Version 2 / 2.5
	{
		if(pHeaderInfo->nLayer ==  1)	//	layer 1
		{
			*bitrate = (*framesize * pHeaderInfo->nSampleRate) / (24 * 1000);
		}
		else
		{
			*bitrate = (*framesize * pHeaderInfo->nSampleRate) / (72 * 1000);
		}
	}	
	return 0;
}

ME_U32 mp3_read_info(mp3_infor_t *file)
{
//	MPEGAudioHeaderInfo pHeaderInfo;
	ME_U32 iret = 0;
	ME_U32 iTimes = 0;
	ME_U32 framelen = 0;	
	ME_U32 iBufLen = 0;
	ME_U32 filelen = 0;
	ME_U32 nBytesLeft = 0;
	ME_U08* stream_buffer = NULL;
	ME_PU08 ptr = NULL;
	char mp3Codec[4] = "mp3";
	unsigned char bTag[4] = "ID3";
	
	file->uloffset = 0;
	g_free_bitrate_flg = 0;
	file->seek_func(file->stream,0L,SEEK_SET);	
	file->seek_func(file->stream,0L,SEEK_END);
	filelen = file->tell_func(file->stream);
	file->seek_func(file->stream,0L,SEEK_SET);

	file->file_length = filelen;
	
	stream_buffer = MMD_Malloc(MAX_FRAME_BUFFER_SIZE);
	if(stream_buffer == NULL)
		return ME_ERR_MALLOC_MEMORY_ERROR;

	file->uloffset = 0;
	iret = mp3_parser_read_buffer(stream_buffer, 10, file);
	if(iret != 10)
		return ME_ERR_READ_FILE_ERROR;

	/* check whether id3v3 exists */
	if (me_cmp(stream_buffer, bTag, 3))
	{
		file->id3_size = 0;
		file->first_frame_offset = 0x00;		
	}
	else
	{
		ptr = &stream_buffer[6];
		iret = mp3_id3v2_get_header_length(&ptr, 4);
		
		if(iret > 0)
		{
			file->id3_size += iret;
			file->first_frame_offset = iret + 0x0a;
		}
		me_printf("\nID3 offset: 0x%x\n", file->first_frame_offset);

	}

	file->uloffset = file->first_frame_offset;
	iret = mp3_parser_read_buffer(stream_buffer, MAX_FRAME_BUFFER_SIZE, file);
	if(iret == 0)
		return ME_FAILE;

	iBufLen = MAX_FRAME_BUFFER_SIZE;
	iTimes = (file->file_length/(MAX_FRAME_BUFFER_SIZE))>((128*1024)/(MAX_FRAME_BUFFER_SIZE))?((128*1024)/(MAX_FRAME_BUFFER_SIZE)):((file->file_length/(MAX_FRAME_BUFFER_SIZE)));//SYNC_TIMES;//modify by yjz 2009/3/5
	if(iTimes ==0)
	{
		iTimes = SYNC_TIMES;
	}
#if 0
	while(iTimes-- > 0)
	{
		//: Modified by antonliu, 2008/06/03
		//iret = MPEGAudioDecoder_HeaderInfo(&pHeaderInfo, stream_buffer, (int *)&iBufLen);
		iret = get_mp3_headinfo(&pHeaderInfo, stream_buffer, (int *)&iBufLen);
		
		if(ERR_OK == iret)
		{
			file->uloffset 			+= iBufLen;
			iBufLen		 		= nBytesLeft;
			file->first_frame_offset	= file->uloffset;
			file->is_mp3			= 1;
			if(pHeaderInfo.nBitRate == 0)
			{
				g_free_bitrate_flg = 1;
				pHeaderInfo.nBitRate = 64;
			}
			break;
		}
		else if(ERR_INDATA_UNDERFLOW == iret)
		{
			file->uloffset	+= iBufLen;
			iret			= mp3_parser_read_buffer(stream_buffer, MAX_FRAME_BUFFER_SIZE, file);
			if(iret == 0)
			{
				file->is_mp3 = 0;
				break;
			}
		}
	}	

	if(!file->is_mp3)
	{
		return ME_FAILE;
	}	

	if(g_free_bitrate_flg == 1)
	{
		 get_free_bitrate(stream_buffer, &pHeaderInfo, &(file->a_frmsize), &(file->a_bitrate));
		me_printf("Free bitrate mp3 file: bitrate = 0x%x, frame_size = 0x%x\n", file->a_bitrate, file->a_frmsize);
	}
	
	switch (pHeaderInfo.nLayer)
	{
	case 3:
		if(pHeaderInfo.nSampleRate < 32000)
			framelen = MPEG2_LAYER3_FRAMELEN;	// MPEG2 and MPEG2.5 Layer3
		else
			framelen = MPEG1_LAYER3_FRAMELEN;	// MPEG1 Layer3
		break;
	case 2:
		framelen = MPEG_LAYER2_FRAMELEN;		// layer2
		break;
	case 1:
		framelen = MPEG_LAYER1_FRAMELEN;		// Layer 1
		break;
	default:
		return ME_FAILE;
		break;
	}

	me_printf("\nchannel = %d,samplerate = %d,a_layer = %d\n\n",pHeaderInfo.nChannels,pHeaderInfo.nSampleRate,pHeaderInfo.nLayer);

	file->a_bits 		= 16;
	file->a_chn 		= 2;//pHeaderInfo.nChannels;
	file->a_frmsize 	= pHeaderInfo.nFarmeSize;
	file->a_smplcount = framelen;
	file->a_smplrate 	= pHeaderInfo.nSampleRate;
	file->a_bitrate 	= pHeaderInfo.nBitRate;
	file->a_duration	= (ME_U32)(file->a_smplcount/file->a_smplrate);		
	me_memcpy(file->a_codec,mp3Codec,sizeof(mp3Codec));
	file->a_layer		= pHeaderInfo.nLayer;
	file->seek_func(file->stream,file->first_frame_offset,SEEK_SET);

	iret = mp3_analyse_stream(file,128,1);		//divide the stream into 64 segments,each segments analyse 1 frame.zhang xuecheng.
	if(!iret)
		me_printf("\ntotaltime = %02d:%02d:%02d,bitrate = %dkbps\n\n",file->a_totaltime/3600,file->a_totaltime/60,file->a_totaltime%60,file->a_bitrate);
	else
		return iret;
	
	file->seek_func(file->stream,file->first_frame_offset,SEEK_SET);
	file->first_frame_offset = file->uloffset;
#else
	while(iTimes-- > 0)
	{
		iret = MP3DecFindSyncWord((void*)( file->mp3DecInfo),
								stream_buffer,
								MAX_FRAME_BUFFER_SIZE,
								2,
								(int *)&iBufLen);
		
		if(mp3dec_ERR_OK == iret)
		{
			file->uloffset 			+= iBufLen;
			iBufLen		 		= nBytesLeft;
			file->first_frame_offset	= file->uloffset;
			file->is_mp3			= 1;
			me_printf("##first_frame_offset=0x%x\n", file->first_frame_offset);
			me_printf("##freeBitrateSlots=0x%x\n", (file->mp3DecInfo)->freeBitrateSlots);
			break;
		}
		else if(mp3dec_ERR_INDATA_UNDERFLOW == iret)
		{
			file->uloffset	+= iBufLen;
			iret			= mp3_parser_read_buffer(stream_buffer, MAX_FRAME_BUFFER_SIZE, file);
			if(iret == 0)
			{
				file->is_mp3 = 0;
				break;
			}
			iBufLen = MAX_FRAME_BUFFER_SIZE;
		}
		//add by yjz 2009/3/5
		else
		{
			file->uloffset	+= MAX_FRAME_BUFFER_SIZE - 3;
			iret			= mp3_parser_read_buffer(stream_buffer, MAX_FRAME_BUFFER_SIZE, file);
			if(iret == 0)
			{
				file->is_mp3 = 0;
				break;
			}
			iBufLen = MAX_FRAME_BUFFER_SIZE;
		}
		///:~
	}	

	if(!file->is_mp3)
	{
		return ME_FAILE;
	}	

	switch ( (file->mp3DecInfo)->layer )
	{
	case 3:
		if((file->mp3DecInfo)->samprate < 32000)
			framelen = MPEG2_LAYER3_FRAMELEN;	// MPEG2 and MPEG2.5 Layer3
		else
			framelen = MPEG1_LAYER3_FRAMELEN;	// MPEG1 Layer3
		break;
	case 2:
		framelen = MPEG_LAYER2_FRAMELEN;		// layer2
		break;
	case 1:
		framelen = MPEG_LAYER1_FRAMELEN;		// Layer 1
		break;
	default:
		return ME_FAILE;
		break;
	}


	file->a_bits 		= 16;
//	(file->mp3DecInfo)->nChans = 2;
	file->a_chn 		=  (file->mp3DecInfo)->nChans;
//	file->a_chn = 2;
	file->a_frmsize 	= (file->mp3DecInfo)->next_header_distance;
	file->a_smplcount = framelen;
	file->a_smplrate 	= (file->mp3DecInfo)->samprate;
	file->a_bitrate 	= ((file->mp3DecInfo)->bitrate) / 1000;
	file->a_duration	= (ME_U32)(file->a_smplcount/file->a_smplrate);		
	me_memcpy(file->a_codec,mp3Codec,sizeof(mp3Codec));
	file->a_layer		=  (file->mp3DecInfo)->layer;
	file->seek_func(file->stream,file->first_frame_offset,SEEK_SET);

	iret = mp3_analyse_stream(file,128,1, framelen);		//divide the stream into 64 segments,each segments analyse 1 frame.zhang xuecheng.
	if(!iret)
	{
		me_printf("\nMPEG Version: %d, Layer: %d\n", (file->mp3DecInfo)->version+1, (file->mp3DecInfo)->layer);
		me_printf("totaltime = %02d:%02d:%02d, bitrate = %dkbps\n\n",file->a_totaltime/3600,file->a_totaltime/60,file->a_totaltime%60,file->a_bitrate);
	}
	else
	{
		return iret;
	}
	
	file->seek_func(file->stream,file->first_frame_offset,SEEK_SET);
	file->first_frame_offset = file->uloffset;
#endif

	MMD_Free(stream_buffer);
	
	return ME_SUCCESS;
}

ME_U32 mp3_analyse_stream(mp3_infor_t* file , ME_U32 segment , ME_U32 frames, ME_U32 frame_len)
{
	MPEGAudioHeaderInfo pHeaderInfo;
	ME_U32	datalength = 0;
	ME_U32	lDelta = 0;
   	ME_U32 iFrames = 0, iBytes = 0, iBitrate = 0;
	ME_U32 seg = 0, frm = 0, Segment = 0, Frames = 0;
	char* file_buf;
	ME_U32 iret = 0;
	long file_offset = 0;
	ME_U32 buf_len = MAX_BUF_LENGTH;
	ME_U32 framesamples = 0;
	datalength = file->file_length;

	/*alloc file buffer*/
	file_buf = (char*)MMD_Malloc(sizeof(char)*MAX_BUF_LENGTH);
	if(file_buf == NULL)		
		return ME_ERR_MALLOC_MEMORY_ERROR;
	
	/*calculate the segment step count*/
	if ( datalength > 16384 )
	{
		lDelta = datalength / segment;
		Segment = segment;
		Frames = frames;
	}
	else
	{
		lDelta = datalength / segment / 4;
		Segment = segment / 4;
		Frames = frames;
	}

	if(!lDelta)
		return ME_FAILE;
	
	/*check whether the file is too small to be spilt into so many segments*/
	//: Modified by antonliu, 2008/06/04
//	while(lDelta < (Frame.framesize+4)*Frames)
	while(lDelta < (file->a_frmsize+4)*Frames)
	{
		Segment /= 2;
		lDelta *= 2;
		if(!Segment)
			return ME_FAILE;
	}
	
	/*seek the file stream to the start*/
	file->seek_func( file->stream , file->first_frame_offset , SEEK_SET);

	/*fill the file buffer*/
	iret = file->read_func(file->stream,file_buf,MAX_BUF_LENGTH);
	if(iret == 0)
		return ME_ERR_READ_FILE_ERROR;

	buf_len = MAX_BUF_LENGTH;
	iret = get_mp3_headinfo(&pHeaderInfo, (unsigned char*)file_buf, (int*)&buf_len); // Set backup_sync_word
	for(seg = 0; seg < Segment; seg++)
	{
		file_offset = seg*lDelta;
		file->seek_func(file->stream,file_offset,SEEK_SET);
		buf_len = MAX_BUF_LENGTH;
		iret = file->read_func(file->stream,file_buf,MAX_BUF_LENGTH);
		frm = Frames;
		
		while(frm)
		{
			//: Modified by antonliu, 2008/06/03
			//iret = SearchSyncword((unsigned char*)file_buf,(int *)&buf_len);
			//iret = search_syncword((unsigned char*)file_buf, (int *)&buf_len);
			iret = get_mp3_headinfo(&pHeaderInfo, (unsigned char*)file_buf, (int*)&buf_len);
			if(iret != 0)
			{
				file_offset += buf_len;
				file->seek_func(file->stream,file_offset,SEEK_SET);
				buf_len = MAX_BUF_LENGTH;
				iret = file->read_func(file->stream,file_buf,MAX_BUF_LENGTH);
#if 0
    				if(iret < MAX_BUF_LENGTH) 
     				goto ana_done;
#else
    				if(iret < MAX_BUF_LENGTH) 
    				{ 
    	 				if(iFrames ==0)
     					{
     						iFrames ++;
      						frm--;
      						framesamples = frame_len;
      						if((file->mp3DecInfo)->freeBitrateFlag == 0)
      						{
       						iBytes += pHeaderInfo.nFarmeSize + 4;
      						}
      						else
      						{
       						iBytes +=  (file->mp3DecInfo)->freeBitrateSlots + 4;
      						}
      						iBitrate += pHeaderInfo.nBitRate;
     					}
     					else
     					{
      						goto ana_done;
     					}
    				}
#endif
			}
			else
			{
			//: Modified by antonliu, 2008/06/04
				//if(freqs[Frame.sampling_frequency] < 32000)
			#if 0
				if(file->a_smplrate < 32000)
					framesamples = 576;				/* MPEG2 and MPEG2.5 Layer3 */
				else
					framesamples = 1152;				/* MPEG1 Layer3 */
			#else
				framesamples = frame_len;	
			#endif
				frm--;
				iFrames++;
				//: Modified by antonliu, 2008/06/04
			#if 0
				iBytes += Frame.framesize+4;
				
				if(file->a_bitrate!= tabsel_123[Frame.lsf][Frame.lay-1][Frame.bitrate_index])
					file->a_vbr = 1;		/* the file is vbr or abr */

				file->a_bitrate += (Frame.framesize+4)*8*(Frame.sampling_frequency)/framesamples;
			#else
				//iBytes += file->a_frmsize+4;
				if((file->mp3DecInfo)->freeBitrateFlag == 0)
				{
					iBytes += pHeaderInfo.nFarmeSize + 4;
				}
				else
				{
					iBytes +=  (file->mp3DecInfo)->freeBitrateSlots + 4;
				}
				iBitrate += pHeaderInfo.nBitRate;
				//me_printf("%d ", pHeaderInfo.nFarmeSize);
				
			#endif
			}
		}
	}

	/*analyze finished*/
ana_done:
	if(iFrames == 0)
		return ME_FAILE;

	//file->pHeaderInfo->nBitRate/=iFrames;
	//: Modified by antonliu, 2008/06/04
	#if 0
	file->a_bitrate = (ME_U64)iBytes*8*(file->a_smplrate)/iFrames/framesamples;	//kbps

	//header->totaltime=(((__int64)(length-g_ID3size) * 8) / (header->bitrate));

	file->a_bitrate = (file->a_bitrate + 500)/1000;

	file->a_totaltime = (((ME_U64)(file->file_length - (ME_U64)file->id3_size) * 8) / (1000*(file->a_bitrate)));
	#else
	//if((file->mp3DecInfo)->freeBitrateFlag == 0)
	{
		file->a_bitrate = (ME_U64)iBytes*8*(file->a_smplrate)/iFrames/framesamples;	//kbps
		file->a_bitrate = (file->a_bitrate + 500)/1000;
		//file->a_bitrate = iBitrate/iFrames;
		file->a_totaltime = (((ME_U64)(file->file_length - file->id3_size)) * 8) / (1000*(file->a_bitrate));
	}
	#endif
	me_printf("file_length=0x%x, id3_size=0x%x, a_bitrate=0x%x\n", file->file_length, file->id3_size, file->a_bitrate);
	/*free file buffer*/
	MMD_Free(file_buf);
	file->seek_func(file->stream,file->first_frame_offset,SEEK_SET);
	
	return ME_SUCCESS;
}

ME_U32 mp3_check_sig(mp3_infor_t *file)
{
	if(file->is_mp3)
		return ME_SUCCESS;
	else
		return ME_FAILE;
}

ME_U32 mp3_getmaxframesize(mp3_infor_t *file,unsigned int *framesize)
{
	*framesize = 4096;
  	return 0;	
}

ME_U32 mp3_getaudioduration(mp3_infor_t *fileinfor, unsigned int frame, unsigned int * duration)
{
	*duration = fileinfor->a_duration;
	return 0;
	
}

ME_U32 mp3_get_framecnt(mp3_infor_t *fileinfor,unsigned int *framecnt)
{
	//*framecnt = *(unsigned int *)(fileinfor->framecnt);
	return 0;
}

ME_U32 mp3_get_framelength(mp3_infor_t *fileinfor,unsigned int frameindex,unsigned int *framelen)
{
	return 0;
}

ME_U32 mp3_readaudio(mp3_infor_t *file,char *audio_buffer, unsigned int *readbytes,unsigned int bufferlength)
{
	*readbytes = file->read_func (file->stream,audio_buffer,bufferlength);
	return 0;
}

//: Add by antonliu, 2008/06/02
#define READ_FRAME_HEADER_SIZE	100
#define MAX_CHECK_LOOP				20


//: Add by antonliu, 2008/06/10
ME_U32 mp3_read_audio_frame_ver2(mp3_infor_t *file, audio_stream_infor* audio_buffer)
{
	ME_U32  buffer_address, buffer_size;
	//ME_U32  result = ME_SUCCESS;
	int iret = 0;
	ME_U32  framesize;
	ME_U32 start_offset=0;
	ME_U32 readsize;
	ME_U32 seek_offset = 0;
	ME_U32 i = 0;
	char frame_header[READ_FRAME_HEADER_SIZE];
	MPEGAudioHeaderInfo frame_header_info;
	int buflen;
	int memfile_cur = 0;
	ME_U32 realstart, reallen, cnt;
	
read_frame_start:	
	buflen = READ_FRAME_HEADER_SIZE;
	framesize = file->a_frmsize;
	readsize = file->read_func(file->stream, frame_header, READ_FRAME_HEADER_SIZE);
	if(readsize != READ_FRAME_HEADER_SIZE)
	{
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;
	}

	iret = get_mp3_headinfo(&frame_header_info, (unsigned char*) frame_header, &buflen);
	if((file->mp3DecInfo)->freeBitrateFlag == 0)
	{
		if(iret != 0)
		{
			//: Add by antonliu, 2008/07/02, for robust
			if(i < MAX_CHECK_LOOP)
			{

				i++;
				goto read_frame_start;
			}
			else
			{
				return ME_FILE_END_OF_AUDIO;
			}
		}	
		framesize = frame_header_info.nFarmeSize;
	}
	else
	{	//	for Free bitrate mp3 file	
		framesize = (file->mp3DecInfo)->freeBitrateSlots;
		buflen = 0;
		if((frame_header[buflen+2] & 0x02) == 0x02)	// Padding bit
		{
			if((frame_header[buflen+1] & 0x06) == 0x06)	//	layer 1
			{
				framesize += 4;
			}
			else		//	layer 2/3
			{
				framesize++;
			}
		}		
	}

	frm_count++;

	memfile_cur = file->tell_func(file->stream);
	seek_offset = memfile_cur -(READ_FRAME_HEADER_SIZE - buflen);
	file->seek_func(file->stream, seek_offset, F_SEEK_SET);
	//me_printf("seek_offset=0x%x\n", seek_offset);
	
	//get buffer address and buffer size
	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;


	buffer_size = audio_buffer->buffer_size-audio_buffer->cursor_write;

	//frame_sector_offset = file->get_offset_sector_func(file->stream, &sector_size);//offset on sector
	//real_toreadbytes = (framesize+frame_sector_offset+(sector_size-1))&(0xffffffff-sector_size+1);//512x or 4x
	
	if(framesize > buffer_size)
	{
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;
	}

	/*
	if( ((MEMORY_FILE*)(file->stream))->FileReadCur + framesize >= file->file_length)
	{
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;	
	}
	*/
	reallen = framesize;
	realstart = (unsigned int)(((MEMORY_FILE*)(file->stream))->FileAddress) + ((MEMORY_FILE*)(file->stream))->FileReadCur;
	start_offset = realstart%4;
	if(start_offset != 0)	
	{
		realstart -= start_offset;
		reallen += start_offset;
	}
	
	if(reallen%4!=0) 
	{
		cnt = 4-(reallen%4);
	}else
	{
		cnt = 0;
	}

	if(framesize+cnt<= (((MEMORY_FILE*)(file->stream))->FileTotalLength - ((MEMORY_FILE*)(file->stream))->FileReadCur))
	{
		reallen += cnt;
		readsize = ME_MEMCPY_H2D((unsigned int)buffer_address, (unsigned char*)(realstart),reallen);
		if(readsize < reallen)
		{
			audio_buffer->is_over = 1;
			return ME_FILE_LAST_OF_AUDIO;
		}
		else
		{
			((MEMORY_FILE*)(file->stream))->FileReadCur += framesize;
		}
	} 
	else 
	{
		reallen = start_offset+((MEMORY_FILE*)(file->stream))->FileTotalLength-((MEMORY_FILE*)(file->stream))->FileReadCur;
		readsize = ME_MEMCPY_H2D((unsigned int)buffer_address, (unsigned char*)(realstart), reallen);
		audio_buffer->is_over = 1;

		audio_buffer->data_length = readsize - cnt;
		audio_buffer->cursor_read = audio_buffer->cursor_write+start_offset;
		audio_buffer->cursor_write+=readsize;	
		audio_buffer->frame_count = 1;		
		
		return ME_FILE_LAST_OF_AUDIO;
	}

	audio_buffer->data_length = readsize - cnt;
	audio_buffer->cursor_read = audio_buffer->cursor_write+start_offset;
	audio_buffer->cursor_write+=readsize;	
	audio_buffer->frame_count = 1;
	
	return ME_SUCCESS;

}



ME_U32 mp3_read_audio_frame_ver3(mp3_infor_t *file, audio_stream_infor* audio_buffer, ME_U32 reg, ME_U32 bit)
{
#if MP3_DECODE_MULTI_FRAME
	ME_U32  result = ME_SUCCESS;
	ME_U32 buffer_address;
	ME_U32  readsize;
	ME_U08* tmp_buf;
	
	
	tmp_buf = me_malloc(audio_buffer->buffer_size);
	if(tmp_buf == NULL)
	{
		return ME_FAILE;
	}

	readsize = file->read_func(file->stream, (char*)tmp_buf, audio_buffer->buffer_size);

	if(readsize == 0)	
		result = 1;
	else
		result = 0;
	
	audio_buffer->data_length = readsize;
	audio_buffer->isnot_empty = readsize;

	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;

	audio_buffer->cursor_read = audio_buffer->cursor_write;
	audio_buffer->cursor_write += readsize;
	audio_buffer->frame_count = 1;	
	
	if(result)
	{
		me_printf("###audio_buffer->data_length=0x%x\n", audio_buffer->data_length);
		audio_buffer->is_over = 1;
		me_free((void*)tmp_buf);
		return ME_FILE_LAST_OF_AUDIO;	
	}
	else
	{
		if(readsize < audio_buffer->buffer_size)	audio_buffer->is_over = 1;
		Vm_WrSram((unsigned int)buffer_address, (char*)tmp_buf, audio_buffer->data_length);
	}

	me_free((void*)tmp_buf);
	return ME_SUCCESS;
#else
	ME_U32  buffer_address, buffer_size;
	int iret = 0;
	ME_U32  framesize;
	ME_U32 sector_size=0;
	ME_U32 frame_sector_offset=0;
	ME_U32 real_toreadbytes=0;
	ME_U32 start_offset=0;
	ME_U32 readoffset, readsize;
	ME_U32 seek_offset = 0;
	ME_U32 i = 0;
	char frame_header[READ_FRAME_HEADER_SIZE];
	MPEGAudioHeaderInfo frame_header_info;
	int buflen;
read_frame_start:	
	buflen = READ_FRAME_HEADER_SIZE;
	framesize = file->a_frmsize;
	readsize = file->read_func(file->stream, frame_header, READ_FRAME_HEADER_SIZE);
	if(readsize != READ_FRAME_HEADER_SIZE)
	{
		audio_buffer->is_over = 1;
		return ME_FILE_END_OF_AUDIO;
	}

	iret = get_mp3_headinfo(&frame_header_info, (unsigned char*) frame_header, &buflen);
	if((file->mp3DecInfo)->freeBitrateFlag == 0)
	{
		if(iret != 0)
		{
			//: Add by antonliu, 2008/07/02, for robust
			if(i < MAX_CHECK_LOOP)
			{

				i++;
				goto read_frame_start;
			}
			else
			{
				return ME_FILE_END_OF_AUDIO;
			}
		}	
		framesize = frame_header_info.nFarmeSize;
	}
	else
	{	//	for Free bitrate mp3 file	
		framesize = (file->mp3DecInfo)->freeBitrateSlots;
		buflen = 0;
		if((frame_header[buflen+2] & 0x02) == 0x02)	// Padding bit
		{
			if((frame_header[buflen+1] & 0x06) == 0x06)	//	layer 1
			{
				framesize += 4;
			}
			else		//	layer 2/3
			{
				framesize++;
			}
		}		
	}

	frm_count++;
	#if 0
	//me_printf("\n----frm_count=%d, frm_size=0x%x----\n", frm_count, framesize);
	if(frm_count <= 10)
	{
		me_printf("\n----frm_count=%d, frm_size=0x%x----\n", frm_count, framesize);
		for(i=0; i<10; i++)
		{
			me_printf("0x%x ", frame_header[buflen+i] );
		}
		me_printf("\n");
	}
	#endif
	seek_offset = ((F_FILE*)(file->stream))->abspos + ((F_FILE*)(file->stream))->relpos -(READ_FRAME_HEADER_SIZE - buflen);
	file->seek_func(file->stream, seek_offset, F_SEEK_SET);
	
	//get buffer address and buffer size
	buffer_address = audio_buffer->buffer_address+audio_buffer->cursor_write;
	start_offset = (buffer_address%4==0)?0:(4-buffer_address%4);
	buffer_address +=start_offset;// 4x
	buffer_size = audio_buffer->buffer_size-audio_buffer->cursor_write-start_offset;

	frame_sector_offset = (ME_U32)(file->get_offset_sector_func(file->stream, &sector_size));//offset on sector
	sector_size += sector_size+sector_size+sector_size;
	real_toreadbytes = (framesize+frame_sector_offset+(sector_size-1))&(0xffffffff-sector_size+1);//512x or 4x


	
	if(real_toreadbytes> buffer_size)
	{
		return ME_FILE_END_OF_AUDIO;
	}
	if( ((F_FILE*)(file->stream))->abspos + ((F_FILE*)(file->stream))->relpos + real_toreadbytes > file->file_length)
	{
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;	
	}
	readsize = file->read_block_func(file->stream, (char*)buffer_address, real_toreadbytes, reg, bit);

	//: Add by antonliu, 2008/06/11
	if(real_toreadbytes != framesize)
	{
		seek_offset = real_toreadbytes-framesize;
		//seek_offset = (((F_FILE*)(file->stream))->abspos) - seek_offset;
		seek_offset = (((F_FILE*)(file->stream))->abspos + ((F_FILE*)(file->stream))->relpos) - seek_offset;
		file->seek_func(file->stream, seek_offset, F_SEEK_SET);
	}

	if(readsize < framesize)
	{	
		audio_buffer->is_over = 1;
		return ME_FILE_END_OF_AUDIO;
	}
//	readsize+=frame_sector_offset%4;
//	frame_sector_offset_4x = frame_sector_offset&0xfffffffc;// 4x
//	readoffset = frame_sector_offset_4x;
	readoffset = frame_sector_offset;//audio do not need 4x startaddress
	//return read size
	audio_buffer->data_length = readsize;
	
	//: Modified by antonliu, 2008/06/10
	audio_buffer->cursor_read = audio_buffer->cursor_write+start_offset+readoffset;
	audio_buffer->cursor_write+=start_offset+readoffset+readsize;	
//	audio_buffer->cursor_read = readoffset;
//	audio_buffer->cursor_write = readoffset;	

	audio_buffer->frame_count = 1;
	return ME_SUCCESS;

#endif
}
//called after filesys has transfered data on SD to 848core.
ME_U32 mp3_read_audio_frame_ver3_post(mp3_infor_t *file, audio_stream_infor *audio_buffer)
{
	return ME_SUCCESS;;
}

ME_U32 mp3_seek(void *fileinfor, unsigned int pos, unsigned int * accesspos)
{
	return 0;
}


