/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	mp3_id3.h
* User for : 	820
* Creat By :   Zhang Xuecheng()                                                                   
* CrateDate: 	2007/08/21
* Last modify : Zhang Xuecheng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Zhang Xuecheng
=======================================================================*/

#include "mp3_id3.h"
#include "me_utils.h"
// ////////////////////////////////////////////////////////////////
#if ME_KM_OBJ_STACK

#endif

ME_U32 mp3_id3_init(void *mediafile, fs_callbacks * fs_filefunc, void **id3infor)
{
	unsigned int result = 0;
	mp3_id3_infor_t *file;

#if ME_KM_FUNNAME
	ME_MESSAGE(("->mp3_id3_init()"));
#endif

	file = (mp3_id3_infor_t *) me_malloc(sizeof(mp3_id3_infor_t));
	if (file)
	{
		me_memset((ME_PU08)file,0,sizeof(mp3_id3_infor_t));//zhangjunwu 2008_03_03
		file->stream = mediafile;
		result = mp3_get_id3_info(file, fs_filefunc);

		if (result)
		{
			mp3_id3_close(file);
			(*id3infor) = NULL;
			return result;
		}
		else
		{
			(*id3infor) = (void *)file;
			return ME_SUCCESS;
		}
	}
	else
	{
		(*id3infor) = NULL;
		return ME_SUCCESS;
	}
}

ME_U32 mp3_id3_close(void *id3infor)
{
	me_free(id3infor);
	return 0;
}

ME_U32 mp3_get_id3_info(mp3_id3_infor_t * file, fs_callbacks * fs_filefunc)
{
	unsigned char *id3_stream_buffer;
	int result = 0;
	int filelen = 0;
	unsigned char *ptr;
	unsigned int id3v2len = 0;
	unsigned char bTag[4] = "ID3";		// id3v2 tag

	id3_stream_buffer = (unsigned char *)me_malloc(128);	// the length of id3v1 is 128 bytes,zhangxuecheng,2007.08.21

	if (!id3_stream_buffer)
		return ME_ERR_MALLOC_MEMORY_ERROR;

	fs_filefunc->seek_func(file->stream, 0L, SEEK_END);
	filelen = fs_filefunc->tell_func(file->stream);
	fs_filefunc->seek_func(file->stream, filelen - 128, SEEK_SET);

	result = fs_filefunc->read_func(file->stream, (char *)id3_stream_buffer, 128);
	if (result != 128)
	{
		me_free(id3_stream_buffer);
		return ME_ERR_READ_FILE_ERROR;
	}

	// Delete by antonliu, 2008/06/03
	result = CheckID3V1Tag(file, id3_stream_buffer, 128);
	//result = 0;

	/*
	 * if id3v1 exist,do not parse id3v2 tag,zhangxuecheng,2007.08.21 
	 */
	if (result == 0)
	{
		me_free(id3_stream_buffer);
		file->bVersion = 1;				// id3v1 found
		return ME_SUCCESS;
	}

	me_memset(id3_stream_buffer, 0, 128);
	fs_filefunc->seek_func(file->stream, 0L, SEEK_SET);
	result = fs_filefunc->read_func(file->stream, (char *)id3_stream_buffer, 10);

	if (result != 10)
	{
		me_free(id3_stream_buffer);
		return ME_ERR_READ_FILE_ERROR;
	}
	
	if (me_cmp((ME_PU08)id3_stream_buffer, (ME_PU08)bTag, 3))
	{	
		me_free(id3_stream_buffer);
		return ME_SUCCESS;		// not find any tag 
	}
	
	file->bVersion = 2;

	ptr = &id3_stream_buffer[6];
	id3v2len = mp3_id3v2_get_header_length(&ptr, 4);

	if (id3v2len)
	{
		me_free(id3_stream_buffer);
		id3_stream_buffer = (unsigned char *)me_malloc(id3v2len);

		fs_filefunc->seek_func(file->stream, 0L, SEEK_SET);
		fs_filefunc->read_func(file->stream, (char *)id3_stream_buffer, id3v2len);
		(file->offset) = 0;
		CheckID3V2Tag(file, fs_filefunc);
	}
	
	me_free(id3_stream_buffer);

	return ME_SUCCESS;
}

unsigned long mp3_id3v2_get_header_length(unsigned char **ptr, unsigned int bytes)
{
	unsigned long value = 0;

	if (bytes != 4 && bytes != 5)
		return 0;

	switch (bytes)
	{
	case 5:
		value = (value << 4) | (*(*ptr)++ & 0x0f);
	case 4:
		value = (value << 7) | (*(*ptr)++ & 0x7f);
		value = (value << 7) | (*(*ptr)++ & 0x7f);
		value = (value << 7) | (*(*ptr)++ & 0x7f);
		value = (value << 7) | (*(*ptr)++ & 0x7f);
	}

	return value;
}

ME_U32 mp3_id3_get_property(void *id3infor, me_property * property)
{
	mp3_id3_infor_t *file;
	file = id3infor;

	if (file->bVersion == 1)
	{
		if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TITLE, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_0.bTitle, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ARSTIST, 6))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_0.bArtist, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ALBUM, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_0.bAlbum, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_YEAR, 4))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_0.bYear, 5);
			property->size = 5;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_COMMENT, 7))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_0.bComment, 30);
			property->size = 28;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TRACK, 5))
		{
			strcpy((char *)property->paddress, "0");
			property->size = 1;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_GENRE, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)g_Genre[file->ID3.V1_0.bGenre], 30);
			property->size = 1;
		}
	}
	else if (file->bVersion == 2)
	{
		if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TITLE, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_1.bTitle, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ARSTIST, 6))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_1.bArtist, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ALBUM, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_1.bAlbum, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_YEAR, 4))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_1.bYear, 5);
			property->size = 5;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_COMMENT, 7))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V1_1.bComment, 28);
			property->size = 28;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TRACK, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)(file->ID3.V1_1.sTrack + 0x30), 1);
			property->size = 1;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_GENRE, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)g_Genre[file->ID3.V1_1.bGenre], 1);
			property->size = 1;
		}
	}
	else if (file->bVersion == 3)
	{
		if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TITLE, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V2.bTitle, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ARSTIST, 6))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V2.bArtist, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_ALBUM, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V2.bAlbum, 30);
			property->size = 30;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_YEAR, 4))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V2.bYear, 5);
			property->size = 5;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_COMMENT, 7))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)file->ID3.V2.bComment, 28);
			property->size = 28;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_TRACK, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)(file->ID3.V2.sTrack + 0x30), 1);
			property->size = 1;
		}
		else if (!me_cmp((ME_PU08)property->type, (ME_PU08)VIM_MEDIA_FILE_PROPERTY_GENRE, 5))
		{
			me_memcpy((char *)property->paddress, (ME_PCS08)g_Genre[file->ID3.V2.bGenre], 1);
			property->size = 1;
		}
	}
	else
	{
		property->size = 0;
	}

	return ME_SUCCESS;
}

ME_U32 mp3_read_buffer(void *buffer, int count, mp3_id3_infor_t * file, fs_callbacks * fs_filefunc)
{
	fs_filefunc->seek_func(file->stream, (file->offset), SEEK_SET);
	return fs_filefunc->read_func(file->stream, buffer, count);
}
/***************************************************************
Description:
Check the mp3 ID3V1 TAG.

  Parameters:
		filelen:    length of mp3 file
		Returns:
		-1  error 
		0   find ID3V1 TAG
		1   not find ID3V1 TAG
****************************************************************/
int CheckID3V1Tag(mp3_id3_infor_t *pTag, unsigned char *buf, long buflen)
{
	UINT8 bTag[4] = "TAG";//id3v1 tag

	if (buflen < 128)
		return -1;	//error

	if( !memcmp(buf,bTag,3))
	{
		memcpy(pTag->ID3.V1_0.bTitle,buf+3,30);  //Title
		memcpy(pTag->ID3.V1_0.bArtist,buf+33,30);//Artist
		memcpy(pTag->ID3.V1_0.bAlbum,buf+63,30);//Album
		memcpy(pTag->ID3.V1_0.bYear,buf+93,4);//Year
		pTag->ID3.V1_1.sZero = *(buf+125);
		if(pTag->ID3.V1_1.sZero)
		{
			pTag->bVersion = 1;  //ID3V1.0
			memcpy(pTag->ID3.V1_0.bComment,buf+97,30);
		}
		else
		{
			pTag->bVersion = 2;//ID3V1.1
			memcpy(pTag->ID3.V1_1.bComment,buf+97,28);
			pTag->ID3.V1_1.sTrack = *(buf+126);
		}
		
		pTag->ID3.V1_1.bGenre = *(buf+127);
		if(pTag->ID3.V1_1.bGenre > MP3_GENRE_NUM )
		{
			pTag->bMp3Genre = (UINT8 *)g_GenreUnkonwn;
		}
		else
		{
			pTag->bMp3Genre = (UINT8 *)g_Genre[pTag->ID3.V1_1.bGenre];
		}

		return 0;//find ID3V1 TAG
	}

	return 1;//Don't find ID3V1 TAG
}
int CheckID3V2Tag(mp3_id3_infor_t * file, fs_callbacks * fs_filefunc)
{
	SINT32 sdSize, sdFrameSize;
	unsigned int dReadLen, dRealLen, dLen;
	UINT8 bTag[4] = "ID3";				// id3v2 tag
	UINT8 buf[32];
	MP3_ID3V2HEADER MP3ID3V2Header;
	// MP3_ID3V2FRAMEHEADER MP3ID3V2FrameHeader;
	id3_byte_t *ptr;
	int first_comment = 0;

	me_memset((ME_PU08)&MP3ID3V2Header, 0, sizeof(MP3_ID3V2HEADER));
	// memset(&MP3ID3V2FrameHeader,0,sizeof(MP3_ID3V2FRAMEHEADER));

	dReadLen = MP3_ID3V2_HEADERSIZE;
	dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
	(file->offset) += dReadLen;
	if (dRealLen < dReadLen)
		return -1;						// error

	if (me_cmp((ME_PU08)buf, (ME_PU08)bTag, 3))
		return 1;						// not find ID3V2 tag 

	MP3ID3V2Header.bMajorVerison = buf[3];
	MP3ID3V2Header.bRevisionVersion = buf[4];
	MP3ID3V2Header.bFlag = buf[5];

	// Compute Mp3 ID3V2 TAG length
	/*
	 * buf[6] &= 127; buf[7] &= 127; buf[8] &= 127; buf[9] &= 127;
	 * sdSize=MP3ID3V2Header.dSize = (((((buf[6] << 7) + buf[7]) << 7) +
	 * buf[8]) << 7) + buf[9]; 
	 */
	ptr = &buf[6];
	sdSize = MP3ID3V2Header.dSize = id3_parse_syncsafe(&ptr, 4);
	MP3ID3V2Header.dSize += MP3_ID3V2_HEADERSIZE;
	if ((MP3ID3V2Header.bMajorVerison == 4) && (MP3ID3V2Header.bFlag & FOOTER_HEADER_MASK))
		MP3ID3V2Header.dSize += FOOTER_HEADER_SIZE;
	file->bVersion = 3;					// ID3V2 flag

	if (MP3ID3V2Header.bMajorVerison == 2)
	{
		if (MP3ID3V2Header.bFlag & 0x40)
			return 0;					// ignore the entire tag if the
		// compression bit is set.
		else
			return parser_ID3V220(file, fs_filefunc, buf, sdSize);
	}
	// Handle extended header
	if (MP3ID3V2Header.bFlag & Extended_HEADER_MASK)
	{
		// get extended header size 
		dReadLen = 4;
		dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
		(file->offset) += dReadLen;
		if (dRealLen < dReadLen)
			return -1;					// error
		ptr = &buf[0];

		if (MP3ID3V2Header.bMajorVerison == 3)
		{								// ID3 V2.3.0
			dLen = id3_parse_uint(&ptr, 4);
			(file->offset) += dLen;
		}
		else
		{								// ID3 V2.4.0
			dLen = id3_parse_syncsafe(&ptr, 4);
			(file->offset) += dLen - 4;
		}
	}
	// Handle MP3ID3v2 Frame Data
	for (; sdSize > 0;)
	{
		// read frame header
		dReadLen = MP3_ID3V2_FRAMEHEADERSIZE;
		dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
		(file->offset) += dReadLen;
		if (dRealLen < dReadLen)
			return -1;					// error

		// get frame size
		ptr = &buf[4];
		if (MP3ID3V2Header.bMajorVerison == 3)
			dLen = id3_parse_uint(&ptr, 4);	// ID3V2.3.0
		else
			dLen = id3_parse_syncsafe(&ptr, 4);	// ID3V2.4.0
		sdFrameSize = dLen;
		if (sdFrameSize > sdSize || sdFrameSize <= 0)
			break;

		// analyze flag bit(Compression and Encryption)
		if (MP3ID3V2Header.bMajorVerison == 3)
		{								// ID3V2.3.0
			if ((buf[9] & 0x80) || (buf[9] & 0x40))
			{
				(file->offset) += sdFrameSize;
				sdSize = sdSize - sdFrameSize;
				continue;
			}
		}
		else
		{								// ID3V2.4.0
			if ((buf[9] & 0x08) || (buf[9] & 0x04))
			{
				(file->offset) += sdFrameSize;
				sdSize = sdSize - sdFrameSize;
				continue;
			}
		}

		// analyze text information frame

		// read Text encoding byte
		(file->offset)++;
		dLen--;

		// Title
		if (me_cmp((ME_PU08)buf, (ME_PU08)ID3_FRAME_TITLE, 4) == 0)
		{
			if (dLen > 29)
				dLen = 29;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			me_memcpy((char *)file->ID3.V2.bTitle, (ME_PCS08)buf, dLen);
		}
		// Album
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3_FRAME_ALBUM, 4) == 0)
		{
			if (dLen > 29)
				dLen = 29;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			me_memcpy((char *)file->ID3.V2.bAlbum, (ME_PCS08)buf, dLen);
		}
		// Artist
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3_FRAME_ARTIST, 4) == 0)
		{
			if (dLen > 29)
				dLen = 29;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error
			me_memcpy((char *)file->ID3.V2.bArtist, (ME_PCS08)buf, dLen);
		}

		// Year
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3_FRAME_YEAR, 4) == 0)
		{
			if (dLen > 4)
				dLen = 4;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error
			me_memcpy((char *)file->ID3.V2.bYear, (ME_PCS08)buf, dLen);
		}
		// Comment
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3_FRAME_COMMENT, 4) == 0)
		{
			// store the first comment frame
			if (first_comment)
				continue;

			// skip Language + $00
			// g_offset+=4;
			dLen -= 4;

			if (dLen > 28)
				dLen = 28;
			dReadLen = dLen;
			(file->offset) += 4;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			(file->offset) -= 4;
			if (dRealLen < dReadLen)
				return -1;				// error

			me_memcpy((char *)file->ID3.V2.bComment, (ME_PCS08)buf, dLen);
			first_comment = 1;
		}
		// Genre
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3_FRAME_GENRE, 4) == 0)
		{
			UINT8 sIndex[6];
			int cnt;

			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			if (buf[0] != '(')
				continue;

			cnt = 0;
			while ((buf[cnt + 1] != ')') && (cnt < 5))
			{
				sIndex[cnt] = buf[cnt + 1];
				cnt++;
			}
			sIndex[cnt] = 0;
			file->ID3.V2.bGenre = (UINT8) atoi((const char *)sIndex);

			if (file->ID3.V2.bGenre > MP3_GENRE_NUM)
			{
				file->bMp3Genre = (UINT8 *) g_GenreUnkonwn;
			}
			else
			{
				file->bMp3Genre = (UINT8 *) g_Genre[file->ID3.V2.bGenre];
			}

		}
		// Track
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3_FRAME_TRACK, 4) == 0)
		{
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			file->ID3.V2.sTrack = (UINT8) atoi((const char *)buf);
		}
		// update size 
		(file->offset) += sdFrameSize - 1;
		sdSize = sdSize - sdFrameSize;
	}

	return 0;							// find ID3 V2 tag
}

int parser_ID3V220(mp3_id3_infor_t * file, fs_callbacks * fs_filefunc, unsigned char *buf, int sdSize)
{
	SINT32 sdFrameSize;
	UINT32 dReadLen, dRealLen, dLen;
	PUINT8 ptr;
	int first_comment = 0;

	// Handle MP3ID3v2 Frame Data
	for (; sdSize > 0;)
	{
		// read frame header
		dReadLen = MP3_ID3V220_FRAMEHEADERSIZE;
		dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
		(file->offset) += dReadLen;
		if (dRealLen < dReadLen)
			return -1;					// error

		// get frame size
		ptr = &buf[3];
		dLen = id3_parse_uint(&ptr, 3);
		sdFrameSize = dLen;
		if (sdFrameSize > sdSize || sdFrameSize <= 0)
			break;

		// analyze text information frame

		// read Text encoding byte
		(file->offset)++;
		dLen--;

		// Title
		if (me_cmp((ME_PU08)buf, (ME_PU08)ID3V220_FRAME_TITLE, 3) == 0)
		{
			if (dLen > 29)
				dLen = 29;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			me_memcpy((char *)file->ID3.V2.bTitle, (ME_PCS08)buf, dLen);
		}
		// Album
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3V220_FRAME_ALBUM, 3) == 0)
		{
			if (dLen > 29)
				dLen = 29;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			me_memcpy((char *)file->ID3.V2.bAlbum, (ME_PCS08)buf, dLen);
		}
		// Artist
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3V220_FRAME_ARTIST, 3) == 0)
		{
			if (dLen > 29)
				dLen = 29;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error
			me_memcpy((char *)file->ID3.V2.bArtist, (ME_PCS08)buf, dLen);
		}

		// Year
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3V220_FRAME_YEAR, 3) == 0)
		{
			if (dLen > 4)
				dLen = 4;
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error
			me_memcpy((char *)file->ID3.V2.bYear, (ME_PCS08)buf, dLen);
		}
		// Comment
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3V220_FRAME_COMMENT, 3) == 0)
		{
			// store the first comment frame
			if (first_comment)
				continue;

			// skip Language + $00
			// g_offset+=4;
			dLen -= 4;

			if (dLen > 28)
				dLen = 28;
			dReadLen = dLen;
			(file->offset) += 4;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			(file->offset) -= 4;
			if (dRealLen < dReadLen)
				return -1;				// error

			me_memcpy((char *)file->ID3.V2.bComment, (ME_PCS08)buf, dLen);
			first_comment = 1;
		}
		// Genre
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3V220_FRAME_GENRE, 3) == 0)
		{
			UINT8 sIndex[8];
			int cnt;

			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			if (buf[0] != '(')
				continue;

			cnt = 0;
			while ((buf[cnt + 1] != ')') && (cnt < 7))
			{
				sIndex[cnt] = buf[cnt + 1];
				cnt++;
			}
			sIndex[cnt] = 0;
			file->ID3.V2.bGenre = (UINT8) atoi((const char *)sIndex);

			if (file->ID3.V2.bGenre > MP3_GENRE_NUM)
			{
				file->bMp3Genre = (UINT8 *) g_GenreUnkonwn;
			}
			else
			{
				file->bMp3Genre = (UINT8 *) g_Genre[file->ID3.V2.bGenre];
			}

		}
		// Track
		else if (me_cmp((ME_PU08)buf, (ME_PU08)ID3V220_FRAME_TRACK, 3) == 0)
		{
			dReadLen = dLen;
			dRealLen = mp3_read_buffer(buf, dReadLen, file, fs_filefunc);
			if (dRealLen < dReadLen)
				return -1;				// error

			file->ID3.V2.sTrack = (UINT8) atoi((const char *)buf);
		}
		// update size 
		(file->offset) += sdFrameSize - 1;
		sdSize = sdSize - sdFrameSize;
	}

	return 0;							// success
}

unsigned long id3_parse_uint(id3_byte_t ** ptr, unsigned int bytes)
{
	unsigned long value = 0;

	// assert(bytes >= 1 && bytes <= 4);

	switch (bytes)
	{
	case 4:
		value = (value << 8) | *(*ptr)++;
	case 3:
		value = (value << 8) | *(*ptr)++;
	case 2:
		value = (value << 8) | *(*ptr)++;
	case 1:
		value = (value << 8) | *(*ptr)++;
	}

	return value;
}

unsigned long id3_parse_syncsafe(id3_byte_t ** ptr, unsigned int bytes)
{
	unsigned long value = 0;

	// assert(bytes == 4 || bytes == 5);

	switch (bytes)
	{
	case 5:
		value = (value << 4) | (*(*ptr)++ & 0x0f);
	case 4:
		value = (value << 7) | (*(*ptr)++ & 0x7f);
		value = (value << 7) | (*(*ptr)++ & 0x7f);
		value = (value << 7) | (*(*ptr)++ & 0x7f);
		value = (value << 7) | (*(*ptr)++ & 0x7f);
	}

	return value;
}
me_id3_t _g_mp3_id3;
ME_U32 id3_mp3_register(void)
{
#if ME_KM_ID3_MP3_EN
	ME_S32 i;
#if ME_KM_OBJ_STACK
	me_id3_t *pthis = &_g_mp3_id3;
#else
	me_id3_t *pthis = (me_id3_t *) me_malloc(sizeof(me_id3_t));
	if (pthis == NULL)
	{
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
#endif

#if ME_KM_FUNNAME
	ME_MESSAGE(("->id3_mp3_register()"));
#endif

	for (i = 0; i < 4; i++)
		pthis->id[i] = 0;
	ME_SPRINTF(pthis->id, "mp3");

	pthis->init = mp3_id3_init;
	pthis->release = mp3_id3_close;
	pthis->get_property = mp3_id3_get_property;

	return me_id3_register(pthis);		/* ×¢²ámp3 id3 */
#else
	return ME_SUCCESS;
#endif
}
