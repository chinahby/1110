#include "stdafx.h"
#include <fstream>

using namespace std;
CString GetAppPath();

void LOG(const char* str)
{
	CString strLogFile = GetAppPath()+"log.txt";
	ofstream file(strLogFile, ios::app);

	if(!file.is_open())return;

	file<<str<<endl;
}