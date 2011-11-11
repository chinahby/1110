#include "stdafx.h"
#include "PDU.h"
#include "Util.h"
#include "bitsarray.h"
#include <stdexcept>
#include "SMS7Bit.h"

using namespace PDU;

struct SMSInfo
{
	BYTE TP_MTI:2;		//!< Message Type Indicator
	BYTE TP_RD:1;		//!< Reject Duplicates
	BYTE TP_VPF:2;		//!< Validity Period Format
	BYTE TP_SRR:1;		//!< Status Report Request
	BYTE TP_UDHI:1;		//!< User Data Header Indicator
	BYTE TP_RP:1;		//!< Reply Path
};



PDUIn::PDUIn(const BYTE* PDUBytes, size_t PDUBytesCount,size_t smsType)
{
	Extract(PDUBytes, PDUBytesCount,smsType);
}

const string& PDUIn::CenterAddress()const
{
	return m_SMSCenterAddress;
}

const string& PDUIn::Number()const
{
	return m_Number;
}

const std::tstring& PDUIn::Text()const
{
	return m_Text;
}

const Timestamp& PDUIn::Time()const
{
	return m_Timestamp;
}

void CheckLength(const BYTE* pos, const BYTE* end)
{
/*
	if(pos > end)
		throw std::length_error("PDUIn failed: length overrange!");
*/
}

char ByteToChar(BYTE b)
{
	if(b>=0 && b<=9)
		return b+'0';
	else if(b>9 && b<16)
		return b-10+'A';
	else 
		ASSERT(0);
	return '0';
}

string ExtarctNumber(vector<BYTE>& numRaw)
{
	string num;
	for(size_t i=0; i<numRaw.size(); ++i)
	{
		BYTE h = (numRaw[i]&0XF0)>>4;
		BYTE l = (numRaw[i]&0X0F);

		num += ByteToChar(l);
		if(0X0F != h)
			num += ByteToChar(h);
	}

	return num;
}

string ExtarctAddress(const BYTE*& pos, const BYTE* end)
{
	string address;

	//地址长度(bytes)
	CheckLength(pos+1, end);
	BYTE addrLen = *pos ++;
	addrLen --; //去掉号码类型一个byte

	//地址类型
	CheckLength(pos+1, end);
	BYTE addrType= *pos ++;

	//检查是否为国际号码
	bool bNationalNumber = (addrType & 0x70) == 0x10;
	if(bNationalNumber) 
		address+='+';

	//短信中心地址
	CheckLength(pos+addrLen, end);
	vector<BYTE> addrRaw;
	addrRaw.reserve(addrLen);
	for(BYTE i=0; i<addrLen; ++i)
	{
		addrRaw.push_back(*pos ++);
	}

	address += ExtarctNumber(addrRaw);

	return address;
}

string ExtarctPhone(const BYTE*& pos, const BYTE* end)
{
	string address;

	//地址长度(bytes)
	CheckLength(pos+1, end);
	BYTE addrLen = *pos ++;
	if(addrLen&1) ++addrLen;
	addrLen /=2;

	//地址类型
	CheckLength(pos+1, end);
	BYTE addrType= *pos ++;

	//检查是否为国际号码
	bool bNationalNumber = (addrType & 0x70) == 0x10;
	if(bNationalNumber) 
		address+='+';

	//号码
	CheckLength(pos+addrLen, end);
	vector<BYTE> addrRaw;
	addrRaw.reserve(addrLen);
	for(BYTE i=0; i<addrLen; ++i)
	{
		addrRaw.push_back(*pos ++);
	}

	address += ExtarctNumber(addrRaw);

	return address;
}


BYTE SwapByteHex2Decimal(BYTE b)
{
	BYTE h = b & 0xF0;
	BYTE l = b & 0X0F;
	
	return l*10 + (h>>4);
}

Timestamp ExtarctTimestamp(const BYTE*& pos, const BYTE* end)
{
	Timestamp tm;
	CheckLength(pos+7, end);

	BYTE b = *pos ++;
	tm.year = SwapByteHex2Decimal(b);
	if(tm.year<70)
		tm.year += 2000;
	else if(tm.year>=70 && tm.year<100)
		tm.year += 1900;

	b = *pos ++;
	tm.month = SwapByteHex2Decimal(b);

	b = *pos ++;
	tm.day = SwapByteHex2Decimal(b);

	b = *pos ++;
	tm.hour = SwapByteHex2Decimal(b);

	b = *pos ++;
	tm.minute = SwapByteHex2Decimal(b);

	b = *pos ++;
	tm.second = SwapByteHex2Decimal(b);

	//忽略时区
	b = *pos ++;

	return tm;
}

static BYTE EncodingType(EStringType stringType)
{
	switch(stringType)
	{
	case STR_DOUBLE_BYTE:
		return 0X08; //Unicode编码
	case STR_ASCII:
		return 0X00; //7 Bit编码
	case STR_SINGLE_BYTE:
	default:
		return 0X04; //8 Bit编码
	}
}

static EStringType StringType(BYTE encode)
{
	bool isUnicode = (encode & 0x08) == 0x08;//UCS2字符集
	bool isSingleBytecode = (encode & 0x04) == 0x04;//8 bit data
		
	if(isUnicode)
	{
		return STR_DOUBLE_BYTE; //Unicode编码
	}
	else if(isSingleBytecode)
	{
		return STR_SINGLE_BYTE; //8 Bit编码
	}
	else
	{
		return STR_ASCII;//7 bit编码，默认编码方式
	}
/*		
	switch(encode)
	{
	case 0X08:
		return STR_DOUBLE_BYTE; //Unicode编码
	case 0X00:
		return STR_ASCII; //7 Bit编码
	case 0X19:
		return STR_ASCII; //Unicode编码
	case 0XF1:
		return STR_ASCII; //7 Bit编码
	default:
		//return STR_SINGLE_BYTE; //8 Bit编码
		return STR_ASCII; //Unicode编码
	}
*/
}

void UnicodeToASCII(LPCWSTR SourceStr, DWORD dwWCharCount, LPSTR pASCII, DWORD dwASCIISize);

std::tstring ExtractText(const BYTE*& pos, const BYTE* end, EStringType stringType,bool isEms)
{
	CheckLength(pos+1, end);

	//得到文本总的字节数
	size_t PDUBytesCount = end -pos;
	
	BYTE len = *pos ++;

	if(stringType == STR_ASCII)
	{
		CheckLength(pos+len*7/8, end);
	}
	else
	{
		CheckLength(pos+len, end);
	}


	BYTE protocolHeadLen=0;
	BYTE oddLen = 0;
	BYTE total = 0;
	BYTE used = 0;
	if(isEms)
	{
		//剩余协议头的长度
		BYTE* pStart = const_cast<BYTE*>(pos) ;
		protocolHeadLen=*pos ++;

		//GSM 03.40 9.2.3.24.1
		BYTE protocolHeadLen=*pos ++;

		//剩下短信标识的长度
		oddLen=*pos ++;

		//这批短信的唯一标志，1或者2个字节
		if((oddLen&0x03)==0x03)
		{
			BYTE flag = *pos ++;
		}
		else
		{
			BYTE flag1 = *pos ++;
			BYTE flag2 = *pos ++;			
		}

             //这批短信的数量
             total =*pos ++;

		//本条是这批短信的第几条
		used =*pos ++;
		//len = PDUBytesCount-protocolHeadLen-2;	
		len = len-(pos - pStart);
	}

	vector<BYTE> data;
	data.reserve(len);
	
	/*
	data.reserve(len-protocolHeadLen+4);
	data.push_back(0x28);
	data.push_back(used);
	data.push_back(0x2F);
	data.push_back(total);
	data.push_back(0x29);*/
	
	if(stringType == STR_DOUBLE_BYTE)
	{
		//ASSERT(len%2 == 0);

		for(BYTE i=0; i<len; i+=2)
		{
			BYTE a = *pos ++;
			BYTE b = *pos ++;
			data.push_back(b);
  			data.push_back(a);
		}

	}
	else
	{
		for(BYTE i=0; i<len; ++i)
		{
			data.push_back(*pos ++);
		}
	}

	if(STR_DOUBLE_BYTE == stringType)
	{
		TCHAR text[512] = {0};
	//	_tcscpy(text,(LPCWSTR)&data[0]);
		LPCTSTR tdata = reinterpret_cast<LPCTSTR>(&data[0]);
		memcpy(text,tdata,len);
		//UnicodeToASCII((LPCWSTR)&data[0], len/2, text, sizeof(text));
		return text;
	}
	else if(STR_ASCII == stringType)
	{
		std::tstring str;
		str.reserve(len);

		CString str2;
		BitsArray ba(&data[0], len);
		CSMS7Bit SMS7Bit;
		size_t t1 = ba.BitsCount() ;
		
		for(size_t i=0; i<ba.BitsCount() && str.size()<len; i+=7)
		{
			size_t t2 = str.size();
			BYTE c = 0;
			ba.GetBits(c, 0, 7, i);
			TCHAR bASCII = 0;
			if(c==27)
			{
				i+=7;
				ba.GetBits(c,0,7,i);		
				SMS7Bit.Decode(27,c,bASCII);
				--len;
			}
			else
			{
				SMS7Bit.Decode(0,c,bASCII);
			}
		
			str += bASCII;
			str2+=bASCII;
		}

		return str;
	}
	else if(STR_SINGLE_BYTE == stringType)
	{
		std::tstring str;
		str.reserve(len);
		for(BYTE i=0; i<len; ++i)
			str += data[i];

		return str;
	}

	ASSERT(0);

	return TEXT("");
}

//接收短信PDU解码
void PDUIn::Extract(const BYTE* PDUBytes, size_t PDUBytesCount,size_t smsType)
{
	const BYTE* pos = PDUBytes;
	const BYTE* end = PDUBytes+PDUBytesCount;
	
	if(( smsType==2)||( smsType == 3))
	{
		
		//短信中心地址
		m_SMSCenterAddress = ExtarctAddress(pos, end);

		//短信属性信息, 跳过不使用
		CheckLength(pos+1, end);
		BYTE info = *pos ++;
		bool isEms = (info & 0x40) == 0x40;//检查是否是扩展短信
		bool isRelativeFormat = (info & 0x10) == 0x10;//为真表示后边的有效期是相对格式，占一个字节
        //如果TP-VPF字段的值为11(表示绝对格式，有效期字段占7字节)，01(表示增强格式，有效期字段占7字节)
		
		bool isAbsoluteFormat = (info & 0x18) == 0x18;//为真表示后边有7字节有效期字段
		bool isEnhancedFormat = (info & 0x08) == 0x08;//为真表示后边有7字节有效期字段
	

		//短信ID, 即reference
		BYTE id = *pos ++;
	
		//对方号码
		m_Number = ExtarctPhone(pos, end);

		//协议标志,跳过不使用
		CheckLength(pos+1, end);
		BYTE protocolIndentifier = *pos ++;

		//编码方法
		CheckLength(pos+1, end);
		BYTE encode = *pos ++;

		//时间戳
		//m_Timestamp = ExtarctTimestamp(pos, end);

		//有效期
		if(isRelativeFormat)
		{
			BYTE period = *pos ++;
		}
		else if(isAbsoluteFormat||isEnhancedFormat)
		{
			BYTE period1 = *pos ++;
			BYTE period2 = *pos ++;
			BYTE period3 = *pos ++;
			BYTE period4 = *pos ++;
			BYTE period5 = *pos ++;
			BYTE period6 = *pos ++;
			BYTE period7 = *pos ++;
		}
		else
		{
			//没有有效期字段
		}
		
		//内容
		m_Text = ExtractText(pos, end, StringType(encode),isEms);	
	}
	else
	{
		//短信中心地址
		m_SMSCenterAddress = ExtarctAddress(pos, end);

		//短信属性信息, 跳过不使用
		CheckLength(pos+1, end);
		BYTE info = *pos ++;
		bool isEms = (info & 0x40) == 0x40;//检查是否是扩展短信


		//bool isRelativeFormat = (info & 0x10) == 0x10;//为真表示后边的有效期是相对格式，占一个字节
        //如果TP-VPF字段的值为11(表示绝对格式，有效期字段占7字节)，01(表示增强格式，有效期字段占7字节)
		
		//bool isAbsoluteFormat = (info & 0x18) == 0x18;//为真表示后边有7字节有效期字段
		//bool isEnhancedFormat = (info & 0x08) == 0x08;//为真表示后边有7字节有效期字段
		

		//对方号码
		m_Number = ExtarctPhone(pos, end);

		//协议标志,跳过不使用
		CheckLength(pos+1, end);
		BYTE protocolIndentifier = *pos ++;

		//编码方法
		CheckLength(pos+1, end);
		BYTE encode = *pos ++;

		//时间戳
		m_Timestamp = ExtarctTimestamp(pos, end);

/*
		//有效期
		if(isRelativeFormat)
		{
			BYTE period = *pos ++;
		}
		else if(isAbsoluteFormat||isEnhancedFormat)
		{
			BYTE period1 = *pos ++;
			BYTE period2 = *pos ++;
			BYTE period3 = *pos ++;
			BYTE period4 = *pos ++;
			BYTE period5 = *pos ++;
			BYTE period6 = *pos ++;
			BYTE period7 = *pos ++;
		}
		else
		{
			//没有有效期字段
		}
*/
		
		//内容
		m_Text = ExtractText(pos, end, StringType(encode),isEms);
	}
}


//***************************************************
// class PDUOut
//***************************************************

PDUOut::PDUOut(const string& number, const string& SMSCenter, const string& text)
:m_Number(number),
 m_SMSCenterAddress(SMSCenter),
 m_Text(text),
 m_PDUDataLen(0)
{
}

void PushNumber(vector<BYTE>& data, const string& number)
{
/*
	size_t numSize = number.size();
	size_t centerAddLen = (numSize%2 )? numSize/2+1: numSize/2;
	centerAddLen += 1; //地址类型一个字节

	data.push_back((BYTE)centerAddLen);

	BYTE addType = number[0] == '+' ? 0X91 : 0x81;
	data.push_back(addType);

	for(size_t i=0; i<number.size(); ++i)
	{
		if(number[i] == '+')
			continue;
		BYTE l = number[i++]-'0';
		BYTE h =( i==number.size() ? 0XF : number[i]-'0'); //非偶数的后面补F
		BYTE num = (h<<4) | l;
		data.push_back(num);
	}
*/
	data.push_back(0);
}

void PushPhone(vector<BYTE>& data, const string& number)
{
	size_t centerAddLen = number.size();
	if(number[0] == '+')
		centerAddLen -= 1;

	data.push_back((BYTE)centerAddLen);

	BYTE addType = number[0] == '+' ? 0X91 : 0x81;
	data.push_back(addType);

	for(size_t i=0; i<number.size(); ++i)
	{
		if(number[i] == '+')
			continue;
		BYTE l = number[i++]-'0';
		BYTE h =( i==number.size() ? 0XF : number[i]-'0'); //非偶数的后面补F
		BYTE num = (h<<4) | l;
		data.push_back(num);
	}
}
		
int ASCIIToUnicode(LPCSTR ASC, DWORD ASCCount,	LPWSTR WCharBuf, DWORD dwWCharSize);

void PushText(vector<BYTE>& data, const string& text, EStringType stringType)
{
	if(STR_DOUBLE_BYTE == stringType)
	{
		WCHAR unicode[1024];
		int nCharCount = ASCIIToUnicode(text.c_str(), text.size(), unicode, sizeof(unicode));
		data.push_back((BYTE)(nCharCount*2));
		for(int i=0; i<nCharCount; ++i)
		{
			WCHAR w = unicode[i];
			data.push_back((w&0xFF00)>>8);
			data.push_back(w&0x00FF);
		}
	}
	else if(STR_ASCII == stringType)
	{
		BitsArray ba;
		int textLen = text.size();
		CSMS7Bit SMS7Bit;
		size_t n27=0;
		for(size_t i=0; i<textLen; ++i)
		{
			BYTE b1 = 0;
			BYTE b2 = 0;

			if(SMS7Bit.Encode(b1,b2,text[i]))
			{
				if(b1 == 27)
				{
					ba.PushBits(b1,0,7);
					ba.PushBits(b2,0,7);
					n27++;
				}
				else
					ba.PushBits(b2,0,7);
			}
			else
				ba.PushBits(text[i], 0, 7);
		}

		//data.push_back((BYTE)ba.BytesCount());
		data.push_back(textLen+n27);
		
		for(i=0; i<ba.BytesCount(); ++i)
		{
			data.push_back(ba.Bytes()[i]);
		}
	}
	else 
	{
		data.push_back((BYTE)text.size());
		for(size_t i=0; i<text.size(); ++i)
		{
			data.push_back(text[i]);
		}
	}
}

vector<BYTE> PDUOut::Encode(BYTE id)
{
	vector<BYTE> data;
	data.reserve(512);

	//短信中心地址
	PushNumber(data, m_SMSCenterAddress);

	int pos = data.size();

	//短信属性信息
	BYTE smsInfo;
	SMSInfo* info = (SMSInfo*)&smsInfo;
	info->TP_MTI = 1;
	info->TP_RD  = 0;
	info->TP_VPF = 2;
	info->TP_SRR = 0;//状态报告要求—TP-SPR（TP-Status-Report-Request）：0—需要报告; 1—不需要报告
	info->TP_UDHI= 0;
	info->TP_RP  = 0;

	data.push_back(smsInfo);

	//短信ID, 即reference
	data.push_back(id);

	//对方号码
	PushPhone(data, m_Number);

	//协议标志
	data.push_back(0X0);	

	//编码格式
	EStringType strType = GetStringType(m_Text.c_str(), m_Text.size());
	BYTE encode = EncodingType(strType);
	data.push_back(encode);

	//有效期
	data.push_back(0XFF);

	//内容长度+内容
	PushText(data, m_Text, strType);

	m_PDUDataLen = data.size()-pos;

	return data;
}