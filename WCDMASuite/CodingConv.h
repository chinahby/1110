// CodingConv.h: interface for the CCodingConv class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(CODINGCONV_H__INCLUDED_)
#define CODINGCONV_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCodingConv  
{
public:
	// buf     - 目标缓冲区
	// buf_len - 目标缓冲区尺寸
	// src     - 原始码
	// 返回值为转换后的目标字节数
	static int GB2312_2_UTF8(char * buf, int buf_len, const char * src, int src_len = 0);
	static int UTF8_2_GB2312(char * buf, int buf_len, const char * src, int src_len = 0);
	
//private:
//	CCodingConv();
//	virtual ~CCodingConv();

};

#endif // !defined(CODINGCONV_H__INCLUDED_)
