// CodingConv.cpp: implementation of the CCodingConv class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CodingConv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

__inline void Gb2312_2_Unicode(unsigned short* dst, const char * src)
{
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, 2, dst, 1);
}

__inline void Unicode_2_UTF8(char* dst, unsigned short* src)
{
	char* pchar = (char *)src;
	
	dst[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	dst[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	dst[2] = (0x80 | ( pchar[0] & 0x3F));
}

__inline void UTF8_2_Unicode(unsigned short* dst, const char * src)
{
	char* uchar = (char *)dst;
	
	uchar[1] = ((src[0] & 0x0F) << 4) + ((src[1] >> 2) & 0x0F);
	uchar[0] = ((src[1] & 0x03) << 6) + (src[2] & 0x3F);
}

__inline void Unicode_2_GB2312(char* dst, unsigned short uData)
{
	WideCharToMultiByte(CP_ACP, NULL, &uData, 1, dst, sizeof(unsigned short), NULL, NULL);
}

int CCodingConv::GB2312_2_UTF8(char * buf, int buf_len, const char * src, int src_len)
{
	if (0 == src_len)
		src_len = strlen(src);

	int j = 0;
	for (int i = 0; i < src_len;)
	{
		if (j >= buf_len - 1)
			break;

		if (src[i] >= 0)
		{
			buf[j++] = src[i++];
		}
		else
		{
			unsigned short w_c = 0;
			Gb2312_2_Unicode(&w_c, src + i);

			char tmp[4] = "";
			Unicode_2_UTF8(tmp, &w_c);
			
			buf[j+0] = tmp[0];
			buf[j+1] = tmp[1];
			buf[j+2] = tmp[2];
			
			i += 2;
			j += 3;
		}
	}

	buf[j] = '\0';

	return j;
}

int CCodingConv::UTF8_2_GB2312(char * buf, int buf_len, const char * src, int src_len)
{
	if (0 == src_len)
		src_len = strlen(src);
	
	int j = 0;
	for (int i = 0; i < src_len;)
	{
		if (j >= buf_len - 1)
			break;

		if (src[i] >= 0)
		{
			buf[j++] = src[i++];
		}
		else
		{
			unsigned short w_c = 0;
			UTF8_2_Unicode(&w_c, src + i);
			
			char tmp[4] = "";
			Unicode_2_GB2312(tmp, w_c);
			
			buf[j+0] = tmp[0];
			buf[j+1] = tmp[1];
			
			i += 3;
			j += 2;
		}
	}
	
	buf[j] = '\0';
	
	return j;
}