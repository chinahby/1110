/*=============================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	util.c  
* User for : 	mp4 demux mux, all base function and file I/O function define in this file
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/10/04
* Last modify : Amanda Deng 2006-xx-xx     
*
* relase history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=============================================================================*/

#include "mp4_parser.h"
#include "..\..\apps\media\media_utils.h"

#ifdef _WIN32

/*******************************************************************************************
Description:
	mp4 memory calloc 
Parameters:
       unsigned int size: memory size(bytes)
Return:	
      memory point
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
void *mp4_calloc( unsigned int n,unsigned int size)
{
	void* memblock=NULL;
	
	memblock = (void*)malloc(n*size);
	MP4_MALLOC_MSG(("address=0x%x,size=%d\n", (unsigned int)memblock,n*size));

	return memblock;
}
/*******************************************************************************************
Description:
	mp4 memory malloc 
Parameters:
       unsigned int size: memory size(bytes)
Return:	
      memory point
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
void *mp4_malloc( unsigned int size)
{
	void* memblock=NULL;
	
	memblock = (void*)malloc(size);
	MP4_MALLOC_MSG(("address=0x%x,size=%d\n", (unsigned int)memblock,size));

	return memblock;
}

/*******************************************************************************************
Description:
	mp4 memory free 
Parameters:
       void *memblock: memory point
Return:	
      	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
void mp4_free( void *memblock )
{
	MP4_FREE_MSG(("address=0x%x\n", (unsigned int)memblock));
	if(memblock)
		free((void *)memblock );
}


uint32 mp4_fileread(void* file, char* buffer, uint32 count)
{
	return fread((void*)buffer, sizeof(char), (long)count, (FILE *)file);
}

uint32 mp4_filewrite(void* file, char* buffer, uint32 count)
{
	return fwrite((const void*)buffer, sizeof(char), (long)count, (FILE*)file);
}

uint32 mp4_fileseek(void* file, uint32 offset, uint32 mode)
{
	return fseek((FILE*)file, offset, mode);
}

uint32 mp4_filetell(void* file)
{
	return ftell((FILE*)file);
}

void*mp4_fileopen(const char *filename,const char *mode)
{
	FILE *file = fopen(filename, mode);
	return (void*)file;
}

uint32 mp4_fileclose(void* file)
{
	return fclose((FILE*)file);
}

uint32 mp4_filedelete(const char *filename)
{
	return 0;
	//return remove(filename);
}
#else
/*******************************************************************************************
Description:
	mp4 memory calloc 
Parameters:
       unsigned int size: memory size(bytes)
Return:	
      memory point
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
void *mp4_calloc( unsigned int n,unsigned int size)
{
	void* memblock=NULL;
	
	memblock = (void*)MMD_Malloc(n*size);
	MP4_MALLOC_MSG(("address=0x%x,size=%d\n", (unsigned int)memblock,size));

	return memblock;
}
/*******************************************************************************************
Description:
	mp4 memory malloc 
Parameters:
       unsigned int size: memory size(bytes)
Return:	
      memory point
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
void *mp4_malloc( unsigned int size)
{
	void* memblock=NULL;
	
	memblock = (void*)MMD_Malloc(size);
	MP4_MALLOC_MSG(("address=0x%x,size=%d\n", (unsigned int)memblock,size));

	return memblock;
}

/*******************************************************************************************
Description:
	mp4 memory free 
Parameters:
       void *memblock: memory point
Return:	
      	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
void mp4_free( void *memblock )
{
	MP4_FREE_MSG(("address=0x%x\n", (unsigned int)memblock));
	if(memblock)
		MMD_Free((void *)memblock );
}

uint32 mp4_fileread(void* file, char* buffer, uint32 count)
{
	return filesys_fileread( file, buffer, count);
}

uint32 mp4_filewrite(void* file, char* buffer, uint32 count)
{
	return filesys_filewrite(file, buffer, count);
}

uint32 mp4_filewrite_core(void* file, char* buffer, uint32 count,unsigned int reg, unsigned int bit)
{
	return filesys_block_filewrite(file, buffer, count, reg, bit);
	//return 1;
}
uint32 mp4_fileseek(void* file, uint32 offset, uint32 mode)
{
	return filesys_fileseek(file, offset, mode);
}

uint32 mp4_filetell(void* file)
{
	return filesys_filetell(file);
}

void*mp4_fileopen(const char *filename,const char *mode)
{
	return filesys_fileopen((void*)filename, (void*)mode);
}

uint32 mp4_fileclose(void* file)
{
	return filesys_fileclose(file);
}

uint32 mp4_filedelete(const char *filename)
{
	return filesys_filedelete((void*)filename);
}

void* mp4_malloc_h2d(unsigned int size)
{
	return (void *)mp_malloc_h2d(size);
}

void mp4_free_h2d(void*  buf)
{
	mp_free_h2d(buf);
}

uint32 mp4_memcpy_h2d(char * str_dst, const char * str_src, unsigned int len)
{
	return mp_memcpy_h2d((char *)str_dst, (char *)str_src, len);
}

uint32 mp4_memcpy_d2h(char * str_dst, const char * str_src, unsigned int len)
{
	//return mp_memcpy_d2h((char *)str_src, (char *)str_dst, len);
	return mp_memcpy_d2h((char *)str_dst,(char *)str_src,len);
}

#endif

/*******************************************************************************************
Description:
	mp4 memory copy 
Parameters:
       void *memblock: memory point
Return:	
      	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
char* mp4_memcopy(char* str_dst, const char* str_src, uint32 len)
{
	return me_memcpy(str_dst, str_src, len);
}

/*******************************************************************************************
Description:
	mp4 memory set 
Parameters:
       void *memblock: memory point
Return:	
      	void
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.04.18		create first version
********************************************************************************************/
void	mp4_memset(char* buf, uint8 val, uint32 size)
{
	memset(buf, val, size);
}


/* Read entire buffer from the preload buffer */
uint32 mp4_read_preload(mp4_infor_t *file, char *data, uint32 size)
{
	uint32  selection_start = file->file_position;
	uint32  selection_end = file->file_position + size;
	uint32  fragment_start, fragment_end, fragment_len=0;
	uint32  srcaddress;
	fragment_start = selection_start - file->preload_start;
	while(selection_start < selection_end)
	{
		/* really user want read data length */
		fragment_len = selection_end - selection_start;
		/* if data not all in preload buffer(this should never hanppend)*/
		if(fragment_start + fragment_len > file->preload_size)
			fragment_len = file->preload_size - fragment_start;
		fragment_end = fragment_start + fragment_len;
	#if 0
		while(fragment_start < fragment_end)
		{
			*data++ = file->preload_buffer[fragment_start++];
		}
	#else
		srcaddress = (uint32)file->preload_buffer;
		srcaddress += fragment_start;
		mp4_memcopy(data, (const char*)srcaddress, fragment_len);
		fragment_start += fragment_len;
	#endif	
		if(fragment_start >= file->preload_size) 
			fragment_start = 0;
		selection_start += fragment_len;
	}
	
	return fragment_len;
}

//by dnn 2008/4/11
#if MP4LIB_VERSION2_FOREHANDLE
uint32 mp4_read_videodata(mp4_infor_t *file, char *data, uint32 size, uint32 reg, uint32 bit)
{
	uint32 readbytes=0;
//	uint32 selection_start = file->file_position;
//	uint32 selection_end = file->file_position + size;
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	uint32 currentpos=0;
#endif
	
#if MP4_DEMUX_MUX_FILE_DEBUG
		currentpos = file->tell_func(file->stream_videodata);
		MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif		
//		if(file->tell_func(file->stream) != file->file_position)
//			file->seek_func(file->stream, file->file_position, SEEK_SET);
#if MP4_DEMUX_MUX_FILE_DEBUG
//		currentpos = file->tell_func(file->stream);
//		MP4_DEMUX_FILE(("want seek file position = %d ",file->file_position));
//		MP4_DEMUX_FILE(("seek result  file position = %d ",currentpos));
#endif		
		readbytes = file->read_block_func(file->stream_videodata, data, size, reg, bit);
	
	
//	file->file_position += readbytes;
	
	return readbytes;
}

uint32 mp4_read_audiodata(mp4_infor_t *file, char *data, uint32 size, uint32 reg, uint32 bit)
{
	uint32 readbytes=0;
//	uint32 selection_start = file->file_position;
//	uint32 selection_end = file->file_position + size;
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	uint32 currentpos=0;
#endif
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	currentpos = file->tell_func(file->stream_audiodata);
	MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif
	readbytes = file->read_block_func(file->stream_audiodata, data, size, reg, bit);
	return readbytes;
}

uint32 mp4_read_videoindx(mp4_infor_t *file, char *data, uint32 size)
{
	uint32 readbytes=0;
//	uint32 selection_start = file->file_position;
//	uint32 selection_end = file->file_position + size;
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	uint32 currentpos=0;
#endif
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	currentpos = file->tell_func(file->stream_videoindx);
	MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif		
	readbytes = file->read_func(file->stream_videoindx, data, size);

	return readbytes;
}

uint32 mp4_read_audioindx(mp4_infor_t *file, char *data, uint32 size)
{
	uint32 readbytes=0;
//	uint32 selection_start = file->file_position;
//	uint32 selection_end = file->file_position + size;
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	uint32 currentpos=0;
#endif
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	currentpos = file->tell_func(file->stream_audioindx);
	MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif		
	readbytes = file->read_func(file->stream_audioindx, data, size);

	return readbytes;
}
#endif
//end

uint32 mp4_read_data(mp4_infor_t *file, char *data, uint32 size)
{
	uint32 readbytes=0;
	uint32 selection_start = file->file_position;
	uint32 selection_end = file->file_position + size;
	
#if MP4_DEMUX_MUX_FILE_DEBUG
	uint32 currentpos=0;
#endif
	
	//if no preload buffer then read data form file
	if(!file->preload_size)
	{
#if MP4_DEMUX_MUX_FILE_DEBUG
		currentpos = file->tell_func(file->stream);
		MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif		
		if(file->tell_func(file->stream) != file->file_position)
			file->seek_func(file->stream, file->file_position, SEEK_SET);
#if MP4_DEMUX_MUX_FILE_DEBUG
		currentpos = file->tell_func(file->stream);
		MP4_DEMUX_FILE(("want seek file position = %d ",file->file_position));
		MP4_DEMUX_FILE(("seek result  file position = %d ",currentpos));
#endif		
		readbytes = file->read_func(file->stream, data, size);
	}
	else
	{
		//have preload buffer
		if((selection_end - selection_start) >= file->preload_size)
		{
			/* Size is larger than preload size.  Should read form file. */
#if MP4_DEMUX_MUX_FILE_DEBUG
			currentpos = file->tell_func(file->stream);
			MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif		
			if(file->tell_func(file->stream) != file->file_position)
			{
				file->seek_func(file->stream, file->file_position, SEEK_SET);
			}
#if MP4_DEMUX_MUX_FILE_DEBUG
			currentpos = file->tell_func(file->stream);
			MP4_DEMUX_FILE(("want seek file position = %d ",file->file_position));
			MP4_DEMUX_FILE(("seek result  file position = %d ",currentpos));
#endif		
			readbytes = file->read_func(file->stream, data, size);

#if MP4_DEMUX_MUX_FILE_DEBUG
			currentpos = file->tell_func(file->stream);
			MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif		
			
		}
		else
		if((selection_start >= file->preload_start) 
			&& (selection_start < file->preload_end) 
			&& (selection_end <= file->preload_end) 
			&& (selection_end > file->preload_start))
		{
			/* Entire range is in preload buffer (read data in preload buffer, need not read from file)*/
			readbytes = mp4_read_preload(file, data, size);
		}
		else
		{
			/* Range is before buffer or over a preload_size away from the end of the buffer. */
			/* Replace entire preload buffer with range. */
#if MP4_DEMUX_MUX_FILE_DEBUG
			currentpos = file->tell_func(file->stream);
			MP4_DEMUX_FILE(("tell file position = %d ",currentpos));
#endif		
			if(file->tell_func(file->stream) != file->file_position)
				file->seek_func(file->stream, file->file_position, SEEK_SET);
#if MP4_DEMUX_MUX_FILE_DEBUG
			currentpos = file->tell_func(file->stream);
			MP4_DEMUX_FILE(("want seek file position = %d ",file->file_position));
			MP4_DEMUX_FILE(("seek result  file position = %d ",currentpos));
#endif		
			readbytes =file->read_func(file->stream, file->preload_buffer, file->preload_size);
			if( readbytes < size)
				{return readbytes;}
			file->preload_start = file->file_position;
			file->preload_end = file->file_position + readbytes;
			file->preload_ptr = 0;
			readbytes = mp4_read_preload(file, data, size);
		}
	}
	file->file_position += readbytes;
	
	return readbytes;
}

uint32 mp4_write_data(mp4_infor_t *file, char *data, uint32 size)
{
	if(file->tell_func(file->stream) != file->file_position) 
		file->seek_func(file->stream, file->file_position, SEEK_SET);
	file->file_position += size;
	return file->write_func( file->stream, data, size);
}

uint32 mp4_write_data_core(mp4_infor_t *file, char *data, uint32 size,unsigned int reg, unsigned int bit)
{
	if(file->tell_func(file->stream) != file->file_position) 
		file->seek_func(file->stream, file->file_position, SEEK_SET);
	file->file_position += size;
	return file->write_block_func( file->stream, data, size,reg, bit);
}

uint32  mp4_get_file_position(mp4_infor_t *file) 
{ 
	return file->file_position; 
}

uint32 mp4_set_file_position(mp4_infor_t *file, uint32  position) 
{
	if(file->rw_mode)
	{	// write file position seek, if file size >0x7fffffff then error
		if (position > 0x7fffffff)
		{
			MP4_DEMUX_ERR(("mp4_set_file_position( file, position = %d )",position));
			return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
		}
		file->file_position = position;
		return MP4_MUX_DEMUX_RET_NOERR;
	}
	else
	{	// read file position seek, if file size > filelength then error
		if (position > file->total_length)
		{
			MP4_DEMUX_ERR(("mp4_set_file_position( file, position = %d )",position));
			MP4_DEMUX_ERR(("file->total_length = %d ",file->total_length));
			return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
		}
		file->file_position = position;
		return MP4_MUX_DEMUX_RET_NOERR;
	}
}


uint32 mp4_test_position(mp4_infor_t *file)
{
	if(file->rw_mode)
	{	
		// test file position, if file size >0x7fffffff then error
		if (mp4_get_file_position(file) > 0x7fffffff)
		{
			MP4_DEMUX_ERR(("file->file_position = %d ",file->file_position));
			return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
		}
	}
	else
	{
		// test file position, if file size > filelength then error
		if (mp4_get_file_position(file) > file->total_length)
		{
			MP4_DEMUX_ERR(("file->file_position = %d ",file->file_position));
			MP4_DEMUX_ERR(("file->total_length = %d ",file->total_length));
			return MP4_MUX_DEMUX_FILE_POINT_OVERFLOW;
		}
	}
	return MP4_MUX_DEMUX_RET_NOERR;
		
}

uint32 mp4_read_fixed32(mp4_infor_t *file, float *value)
{
	uint32 a, b, c, d, result;
	char data[4];

	result = mp4_read_data(file, data, 4);

	a = (uint8)data[0];
	b = (uint8)data[1];
	c = (uint8)data[2];
	d = (uint8)data[3];
	
	a = (a << 8) + b;
	b = (c << 8) + d;
	
	*value = (float)a + (float)b / 65536;
	
	if( result == 4)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_write_fixed32(mp4_infor_t *file, float number)
{
	char data[4];
	uint32 a, b;

	a = (uint32)number;
	b = (uint32)((number - a) * 65536);
	data[0] = (unsigned char)(a >> 8);
	data[1] = (unsigned char)(a & 0xff);
	data[2] = (unsigned char)(b >> 8);
	data[3] = (unsigned char)(b & 0xff);

	if( mp4_write_data(file, data, 4) == 4)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_write_uint64(mp4_infor_t *file, uint64 value)
{
	char data[8];
	int32 i;

	for (i = 7; i >= 0; i--) 
	{
		data[i] =(unsigned char)(value & 0xff);
		value >>= 8;
	}
	
	if( mp4_write_data(file, data, 8) == 8)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_write_uint32(mp4_infor_t *file, uint32  value)
{
	char data[4];

	data[0] = (unsigned char)((value & 0xff000000) >> 24);
	data[1] = (unsigned char)((value & 0xff0000) >> 16);
	data[2] = (unsigned char)((value & 0xff00) >> 8);
	data[3] = (unsigned char)(value & 0xff);

	if( mp4_write_data(file, data, 4) == 4)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_write_char32(mp4_infor_t *file, char *string)
{
	if( mp4_write_data(file, string, 4) == 4)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}


uint32 mp4_read_fixed16(mp4_infor_t *file, float *value)
{
	char data[2];
	uint32 result;
	
	result = mp4_read_data(file, data, 2);
	*value = (float)data[0] + (float)data[1] / 256;

	if( result == 2)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_write_fixed16(mp4_infor_t *file, float number)
{
	char data[2];
	uint32 a, b;

	a = (uint32)number;
	b = (uint32)((number - a) * 256);
	
	data[0] = (unsigned char)(a & 0xff);
	data[1] = (unsigned char)(b & 0xff);
	
	if( mp4_write_data(file, data, 2) == 2)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_read_uint64(mp4_infor_t *file, uint64 *value)
{
	char data[8];
	uint64 tempvalue=0;
	uint32 i, result;

	result = mp4_read_data(file, data, 8);
	for (i = 0; i < 8; i++) 
	{
		tempvalue |= ((uint8)data[i]) << ((7 - i) * 8);
	}
	*value = tempvalue;

	if( result == 8)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32  mp4_read_uint32(mp4_infor_t *file, uint32 *value)
{
	uint32 tempvalue, result;
	char data[4];
	
	result = mp4_read_data(file, data, 4);
	
	tempvalue = (uint8)data[0]<<24;
	tempvalue = tempvalue |((uint8)data[1]<<16);
	tempvalue = tempvalue |((uint8)data[2]<<8);
	tempvalue = tempvalue | (uint8)data[3];
	
	*value =(uint32)tempvalue;
	
	if( result == 4)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}


uint32  mp4_read_uint24(mp4_infor_t *file, uint32 *value)
{
	uint32 tempvalue, result;
	char data[3];
	
	result = mp4_read_data(file, data, 3);
	
	tempvalue = (uint8)data[0]<<16;
	tempvalue = tempvalue |((uint8)data[1]<<8);
	tempvalue = tempvalue | (uint8)data[2];
	
	*value =(uint32)tempvalue;
	
	if( result == 3)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_write_uint24(mp4_infor_t *file, uint32  number)
{
	char data[4];
	
	data[0] =  (unsigned char)((number & 0xff0000) >> 16);
	data[1] =  (unsigned char)((number & 0xff00) >> 8);
	data[2] =  (unsigned char)((number & 0xff));
	
	if( mp4_write_data(file, data, 3) == 3)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}

}

uint32 mp4_read_uint16(mp4_infor_t *file, uint32 *value)
{
	uint32 tempvalue, result;
	char data[2];
	
	result = mp4_read_data(file, data, 2);
	
	tempvalue = (uint8)data[0]<<8;
	tempvalue = tempvalue | (uint8)data[1];
	
	*value =(uint32)tempvalue;
	
	if( result == 2)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_write_uint16(mp4_infor_t *file, uint32 number)
{
	char data[2];
	
	data[0] = (unsigned char)((number & 0xff00) >> 8);
	data[1] = (unsigned char)(number & 0xff);
	
	if( mp4_write_data(file, data, 2) == 2)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

uint32 mp4_read_char32(mp4_infor_t *file, char *string)
{
	if( mp4_read_data(file, string, 4) == 4)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}

void mp4_copy_char32(char *output, char *input)
{
	*output++ = *input++;
	*output++ = *input++;
	*output++ = *input++;
	*output = *input;
}

uint32 mp4_match_char32(char *input, char *output)
{
	if(input[0] == output[0] &&input[1] == output[1] 
	&& input[2] == output[2] &&input[3] == output[3])
		return TRUE;
	else 
		return FALSE;
}

uint32  mp4_swap_uint32(uint32  value)
{
	unsigned char data[4];
	
	data[0] = (value & 0xff000000) >> 24;
	data[1] = (value & 0xff0000) >> 16;
	data[2] = (value & 0xff00) >> 8;
	data[3] = value & 0xff;
	
	return  data[0] +data[1]*256+data[2]*256*256+data[3] *256*256*256 ;
}

uint32 mp4_read_char(mp4_infor_t *file, uint32 *value)
{
	char output;
	uint32 result;
	
	result = mp4_read_data(file, &output, 1);
	
	*value = (uint32)output;
	
	if( result== 1)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}

}

uint32 mp4_write_char(mp4_infor_t *file, uint32 x)
{
	char data[1];
	
	data[0] = (unsigned char)(x & 0xff);
	
	if( mp4_write_data(file, data, 1) == 1)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}
}


uint32 mp4_read_pascal(mp4_infor_t *file, char *data)
{
	uint32 len, result;
	
	result = mp4_read_char(file, &len);
	if(result !=1)
		{return MP4_DEMUX_READ_FILE_ERROR;}

	if(!len)
	{
		data[0]=0;
		return MP4_MUX_DEMUX_RET_NOERR;
	}
	result = mp4_read_data(file, data, len);
	data[len] = 0;
	if( result == len)
		{return MP4_MUX_DEMUX_RET_NOERR;}
	else
		{return MP4_DEMUX_READ_FILE_ERROR;}

}

uint32 mp4_write_pascal(mp4_infor_t *file, char *data)
{
	uint32 result;
	char   len;
	
	len =(char)strlen(data);
	result = mp4_write_data(file, &len, 1);
	if( result ==1)
	{
		result = mp4_write_data(file, data,(uint32)len);
		if( result == (uint32)len)
			{return MP4_MUX_DEMUX_RET_NOERR;}
		else
			{return MP4_MUX_WRITE_FILE_ERROR;}
	}
	else
		{return MP4_MUX_WRITE_FILE_ERROR;}
}


uint32 mp4_read_descr_length(mp4_infor_t *file, uint32 *length)
{
	uint32 b, numBytes = 0;
	uint32 templength = 0;

	do 
	{
		mp4_read_char(file, &b);
		numBytes++;
		templength = (templength << 7) | (b & 0x7F);
	} while ((b & 0x80) && numBytes < 4);

	*length = templength;

	return MP4_MUX_DEMUX_RET_NOERR;
}

uint32 mp4_write_descr_length(mp4_infor_t *file, uint32 length, uint32 compact)
{
	uint8 b;
	int32 i;
	uint32 numBytes;

	if (compact) 
	{
		if (length <= 0x7F) 
		{
			numBytes = 1;
		} 
		else if (length <= 0x3FFF)
		{
			numBytes = 2;
		} 
		else if (length <= 0x1FFFFF) 
		{
			numBytes = 3;
		} 
		else 
		{
			numBytes = 4;
		}
	} 
	else 
	{
		numBytes = 4;
	}

	for (i = numBytes-1; i >= 0; i--)
	{
		b = (length >> (i * 7)) & 0x7F;
		if (i != 0) 
		{
			b |= 0x80;
		}
		mp4_write_char(file, b);
	}

	return 0; 

}

void mp4_print_chars(char *desc, char *input, uint32 len)
{
	uint32 i;
	MP4_DEMUX_MSG(("%s", desc));
	for(i = 0; i < len; i++) 
		MP4_DEMUX_MSG(("%c", input[i]));
	MP4_DEMUX_MSG(("\n"));
}

uint32 mp4_current_time(void)
{
	return 0;
}

/*******************************************************************************************
Description:
	get n bit data value form bitstream, position form offset 
Parameters:	
	unsigned char* bitsstream:bit stream data point
	uint32 n, bit counter<=32
	uint32 offset: bit offset form bitstream start
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2008.03.10		create first version
********************************************************************************************/
uint32 mp4_bitstream_get_bits(unsigned char* bitsstream, uint32 n, uint32 offset)
{
	uint32 bit_left, tmp_value;
	unsigned char *  data;
	
	bit_left	= offset%8;
	data		= (unsigned char *)((uint32)bitsstream+offset/8);

	if((bit_left+n)<=8)
	{
		tmp_value = (*data)<<bit_left;
		tmp_value &= 0xff;
		tmp_value = tmp_value>>(8-n);
	}
	else if((bit_left+n)<=16)
	{
		tmp_value = (*data++)<<8;
		tmp_value |= (*data);
		tmp_value = tmp_value<<bit_left;
		tmp_value &= 0xffff;
		tmp_value = tmp_value>>(16-n);
	}
	else if((bit_left+n)<=24)
	{
		tmp_value = (*data++)<<16;
		tmp_value |= (*data++)<<8;
		tmp_value |= (*data);
		tmp_value = tmp_value<<bit_left;
		tmp_value &= 0xffffff;
		tmp_value = tmp_value>>(24-n);
	}
	else if((bit_left+n)<=32)
	{
		tmp_value = (*data++)<<24;
		tmp_value |= (*data++)<<16;
		tmp_value |= (*data++)<<8;
		tmp_value |= (*data);
		tmp_value = tmp_value<<bit_left;
		tmp_value &= 0xffffffff;
		tmp_value = tmp_value>>(32-n);
	}
	else
	{
		tmp_value = (*data++)<<24;
		tmp_value |= (*data++)<<16;
		tmp_value |= (*data++)<<8;
		tmp_value |= (*data++);
		tmp_value = tmp_value<<bit_left;
		tmp_value &= 0xffffffff;
		tmp_value = tmp_value+((*data)>>(8-bit_left));
		tmp_value = tmp_value>>(32-n);
	}
	return tmp_value;
}

/*******************************************************************************************
Description:
	set n bit data value to bitstream, position form offset 
Parameters:	
	unsigned char* bitsstream:bit stream data point
	uint32 value: set value(0~65535)
	uint32 n, bit counter<=32
	uint32 offset: bit offset form bitstream start
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2008.03.10		create first version
********************************************************************************************/
uint32 mp4_bitstream_set_bits(unsigned char* bitsstream, uint32 value, uint32 n, uint32 offset)
{
	uint32 bit_left, tmp_value;
	uint32 lsb_value, msb_value;
	unsigned char *  data;
	
	bit_left	= offset%8;
	data		= (unsigned char * )((uint32)bitsstream+offset/8);

	if((bit_left+n)<=32)
	{
		tmp_value = (*data++)<<24;
		tmp_value |= (*data++)<<16;
		tmp_value |= (*data++)<<8;
		tmp_value |= (*data);
		msb_value = tmp_value>>(32-bit_left);
		msb_value = msb_value<<n;
		msb_value |= value;
		msb_value = msb_value<<(32-bit_left-n);
		lsb_value = tmp_value<<(bit_left+n);
		lsb_value = lsb_value>>(bit_left+n);
		msb_value |= lsb_value;
		data		 = (unsigned char * )((uint32)bitsstream+offset/8);
		*data++  = (msb_value>>24)&0xff;
		*data++  = (msb_value>>16)&0xff;
		*data++  = (msb_value>>8)&0xff;
		*data  = (msb_value)&0xff;
	}
	else
	{
		tmp_value = (*data++)<<24;
		tmp_value |= (*data++)<<16;
		tmp_value |= (*data++)<<8;
		tmp_value |= (*data);
		lsb_value = value>>bit_left;
		msb_value = tmp_value>>(32-bit_left);
		msb_value = msb_value<<(32-bit_left);
		msb_value |= lsb_value;
		data		 = (unsigned char * )((uint32)bitsstream+offset/8);
		*data++  = (msb_value>>24)&0xff;
		*data++  = (msb_value>>16)&0xff;
		*data++  = (msb_value>>8)&0xff;
		*data++  = (msb_value)&0xff;
		
		tmp_value = (*data)<<24;
		msb_value = value<<(32-bit_left);
		lsb_value = tmp_value<<(n+bit_left-32);
		lsb_value = lsb_value>>(n+bit_left-32);
		msb_value |= lsb_value;
		*data = (msb_value>>24)&0xff;
	}
	return MP4_MUX_DEMUX_RET_NOERR;
}

/*******************************************************************************************
Description:
	set n bit data value to bitstream, position form offset 
Parameters:	
	unsigned char* bitsstream:bit stream data point
	uint32 *datalength: stream data length, can be changed of this function
	uint32 n, bit counter==8/16/32/40.....
	uint32 offset: bit offset form bitstream start
Return:
	MP4_MUX_DEMUX_RET_NOERR
	MP4_MUX_DEMUX_INVALID_PARAMETER
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2008.03.10		create first version
********************************************************************************************/
uint32 mp4_bitstream_delete_bits(unsigned char* bitsstream, uint32 *datalength, uint32 n, uint32 offset)
{
	unsigned char* srcdata;
	unsigned char* dstdata;
	uint32 bit_left, byte_offset;
	uint32 lsb_value, msb_value;
	
	bit_left	= offset%8;
	byte_offset = offset/8;
	srcdata 	= (unsigned char * )((uint32)bitsstream+byte_offset);
	
	msb_value = (*srcdata);
	msb_value = msb_value>>(8-bit_left);
	msb_value &= 0xff;
	msb_value = msb_value<<(8-bit_left);
	srcdata += n/8;
	*datalength -= n/8;
	lsb_value = (*srcdata);
	lsb_value = lsb_value<<(bit_left);
	lsb_value &= 0xff;
	lsb_value = lsb_value>>(bit_left);
	msb_value |= lsb_value;
	
	dstdata 	= (unsigned char * )((uint32)bitsstream+byte_offset);
	while(byte_offset < *datalength)
	{
		*dstdata++ = *srcdata++;
		byte_offset++;
	}
	dstdata 	= (unsigned char * )((uint32)bitsstream+offset/8);
	 *dstdata = (msb_value)&0xff;
	return MP4_MUX_DEMUX_RET_NOERR;
}

uint8 mp4_log2_tab16[16] =  { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

uint32 mp4_log2bin(uint32 value)
{
	int n = 0;
	if (value & 0xffff0000) {
		value >>= 16;
		n += 16;
	}
	if (value & 0xff00) {
		value >>= 8;
		n += 8;
	}
	if (value & 0xf0) {
		value >>= 4;
		n += 4;
	}
	return n + mp4_log2_tab16[value];
}
