#include "stdafx.h"
#include "bitsarray.h"

BitsArray::BitsArray():m_Size(0), m_Free(0)
{
	m_Data.reserve(512);
}

BitsArray::BitsArray(unsigned char* bytes, size_t bytesCount)
:m_Size(bytesCount*8), m_Free(0)
{
	m_Data.reserve(bytesCount);
	for(size_t i=0; i<bytesCount; ++i)
	{
		m_Data.push_back(bytes[i]);
	}
}

void BitsArray::Grow()
{
	m_Data.push_back(0);
	m_Free += 8;
}

std::string BitsArray::ToBinaryString()const
{
	std::string str;
	str.reserve(m_Size);
	const unsigned char* bytes = &m_Data[0];
	while(str.size() < m_Size)
	{
		unsigned char b = 0x01;
		for(size_t i=0; i<8 && str.size() < m_Size; ++i)
		{
			if(*bytes & b)
				str += '1';
			else
				str += '0';
			b <<= 1;
		}
		++bytes;
	}

	return str;
}

char BitsArray::BCD2Char(unsigned char bcd)
{
	assert(bcd<16);
	if(bcd<=9)
		return '0'+bcd;
	else
		return 'A'+bcd-10;
}

std::string BitsArray::Byte2String(unsigned char byte)
{
	std::string str;
	unsigned char h = byte >> 4;
	unsigned char l = byte & 0X0F;
	str += BCD2Char(h);
	str += BCD2Char(l);
	return str;
}

std::string BitsArray::ToHexString()const
{
	std::string str;
	str.reserve(m_Data.size()*2);
	const unsigned char* bytes = &m_Data[0];
	for(size_t i=0; i<m_Data.size(); ++i)
	{
		str += Byte2String(m_Data[i]);
	}

	return str;
}

static const unsigned char s_mask[] = 
{
	0x00, //0000 0000
	0x01, //0000 0001
	0x03, //0000 0011
	0x07, //0000 0111
	0x0F, //0000 1111
	0x1F, //0001 1111
	0x3F, //0011 1111
	0x7F, //0111 1111
	0xFF, //1111 1111
};

static unsigned char PastByte(unsigned char dest, size_t to,size_t count, unsigned char src, size_t from)
{
	ASSERT(from+count <=8);
	ASSERT(to+count <=8);

	unsigned char b = src & (s_mask[count]<<from);
	unsigned char c = dest & ~(s_mask[count]<<to);
	if(from<to)
		b <<= to-from;
	else
		b >>= from-to;

	return b | c;
}

void BitsArray::PushBits(unsigned char val, size_t pos, size_t count)
{
	assert(pos+count <= 8);
	if(pos+count > 8)
	{
		throw std::invalid_argument("bits position and count cannot excel 8");
	}

	while(m_Free<count)Grow();
	
	size_t left = m_Free%8;
	if(left)
	{
		unsigned char& dest = m_Data[m_Size/8];
		size_t nCopy = left<count? left : count;

		dest = PastByte(dest, 8-left, nCopy, val, pos);
		m_Free -= nCopy;
		count -= nCopy;
		pos += nCopy;
		m_Size += nCopy;
	}

	if(count)
	{
		assert(!(m_Size%8));

		unsigned char& org = m_Data[m_Size/8];
		org = PastByte(org, 0, count, val, pos);

		m_Free -= count;
		m_Size += count;
	}
}

size_t BitsArray::BitsCount()const
{
	return m_Size;
}

void BitsArray::GetBits(unsigned char& dest, size_t destPos, size_t count, size_t srcPos)
{
	assert(destPos+count <=8);
	assert(srcPos+count <=m_Size);
	if(destPos+count > 8)
	{
		throw std::invalid_argument("bits position and count cannot excel 8");
	}
	if(srcPos+count > m_Size)
	{
		throw std::length_error("read position and count cannot excel total size");
	}

	size_t left = 8-srcPos%8;
	if(left)
	{
		unsigned char src = m_Data[srcPos/8];
		size_t nCopy = left<count? left:count;
		dest = PastByte(dest, destPos, nCopy, src, 8-left);

		destPos += nCopy;
		srcPos += nCopy;
		count -= nCopy;
	}

	if(count)
	{
		assert(srcPos%8 == 0);
		unsigned char src = m_Data[srcPos/8];
		dest = PastByte(dest, destPos, count, src, 0);
	}
}

size_t BitsArray::BytesCount()const
{
	return m_Data.size();
}

const unsigned char* BitsArray::Bytes()const
{
	return &m_Data[0];
}

