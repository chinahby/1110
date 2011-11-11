#ifndef MY_F_STREAM
#define MY_F_STREAM

#include <fstream>

class MyStream: public std::ifstream
{
public:
	MyStream(const char* filename)
		: std::basic_ifstream<char>(filename)
	{
	}

	/** 读取一行，遇到\n结尾或者缓冲满
	*std::ifstream::getline当缓存不够时会设置failbit，导致下一次读失败
	*所以写这个函数来代替
	*@return 返回读取到缓冲中的字节数
	*/
	size_t readline(char buf[], size_t buf_size)
	{
		size_t nRead = 0;
		char c;
		while(!eof() && nRead<buf_size)
		{
			read(&c, 1);
			if(fail())
				break;
			if(c=='\n')
			{
				break;
			}
			if(c!='\r')
				buf[nRead++] = c;
		}

		return nRead;
	}
};

#endif