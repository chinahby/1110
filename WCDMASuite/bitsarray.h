#pragma once
#include <vector>
#include <cassert>
#include <string>
#include <stdexcept>

class BitsArray
{
	std::vector<unsigned char> m_Data;
	size_t m_Size;
	size_t m_Free;
public:
	/** \brief 默认构造函数 */
	BitsArray();
	/** \brief 从一个字节数组构造
	*@param bytes 字节数组的头指针
	*@param bytesCount 字节数组的长度(字节数)
	*/
	BitsArray(unsigned char* bytes, size_t bytesCount);

	/** \brief 转换成二进制字符串*/
	std::string ToBinaryString()const;

	/** \brief 转换成16进制字符串*/
	std::string ToHexString()const;

	/** \brief 添加若干比特到尾部
	*@param val 待添加的数值字节
	*@param pos 待添加比特在数值字节字节中的位置，范围为0-7
	*@param count 待添加的比特数,与pos相加不能超过8
	*/
	void PushBits(unsigned char val, size_t pos, size_t count);

	/** \brief 提取若干个比特
	*@param dest 存放提取的比特的字节
	*@param destPos 提取的比特在字节中的存放位置，范围为0-7
	*@param count 提取的比特数，与destPos相加不能超过8
	*@param srcPos 开始提取的位置
	*/
	void GetBits(unsigned char& dest, size_t destPos, size_t count, size_t srcPos);

	/** \brief 本队象中存放的比特数*/
	size_t BitsCount()const;

	/** \breif 占用的字节长度 */
	size_t BytesCount()const;

	/** \占用的字节数组头指针 */
	const unsigned char* Bytes()const;

private:
	void Grow();
	static std::string Byte2String(unsigned char byte);
	static char BCD2Char(unsigned char bcd);
};