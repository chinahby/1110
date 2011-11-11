#ifndef LOG_H_5371237E8A9B45ebB6F6E8E9649A5C86
#define LOG_H_5371237E8A9B45ebB6F6E8E9649A5C86

#define DEBUG_LOG


#ifndef DEBUG_LOG
	#define LOGOUT(X)
	#define LOGOUT2(X,Y)
	#define LOGOUT3(x,y,z)
#else
	#include <fstream>
	CString GetAppPath();
	static CString GetLogFileName()
	{
		return GetAppPath()+"LOGOUT.txt";
	}
	template<typename T>
	void LOGOUT(const T& x)
	{
		std::ofstream file(GetLogFileName(), std::ios::app);
		if(file.is_open())
		{
			file<<x<<std::endl;
		}
	}

	template<typename T1, typename T2>
	void LOGOUT2(const T1& x, const T2& y)
	{
		std::ofstream file(GetLogFileName(), std::ios::app);
		if(file.is_open())
		{
			file<<x<<y<<std::endl;
		}
	}

	
	template<typename T1, typename T2, typename T3>
	void LOGOUT3(const T1& x, const T2& y, const T3& z)
	{
		std::ofstream file(GetLogFileName(), std::ios::app);
		if(file.is_open())
		{
			file<<x<<y<<z<<std::endl;
		}
	}

#endif

#endif
