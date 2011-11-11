/*********************************************************************
	A utility class to help walking a directory tree.
	BY guanxiaopeng, 2003.3


	This is a template class.The argument class must supply one or 
	more member function(s) whose prototype is :

		bool T::ProcessFunc(const string& ,const WIN32_FIND_DATA&);

	Iff the member fucntion return false,the walking will stop.

**********************************************************************/

#ifndef DIRECTORY_TREE_WALKER_HEADER_BB78E5B023404e98986751751CAE6448
#define DIRECTORY_TREE_WALKER_HEADER_BB78E5B023404e98986751751CAE6448

#include <windows.h>
#include <string>

using namespace std;

class CDirHelper
{
public:
	static bool IsDirectory(const WIN32_FIND_DATA& fData)
	{
		return (fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)&&
				strcmp(fData.cFileName,".") &&
				strcmp(fData.cFileName,"..");
	}
	static bool IsFile(const WIN32_FIND_DATA& fData)
	{
		return !(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)&&
				strcmp(fData.cFileName,".") &&
				strcmp(fData.cFileName,"..");
	}

};

template<typename T>
class CDirWalker
{
public:
	void WalkDirTree(const string& rootDirFullName,
				bool (T::*pProcessFunc)(const string& ,const WIN32_FIND_DATA&),
				T& t, bool bRecursive = false)
	{
		WIN32_FIND_DATA findData;
		string sDir = rootDirFullName;
		if(rootDirFullName[rootDirFullName.size()-1] != '\\')
			sDir += '\\';

		string sFileName = sDir+"*.*";
		HANDLE hFind = ::FindFirstFile(sFileName.c_str(),&findData);
		if(hFind == INVALID_HANDLE_VALUE)return;

		do
		{
			bool bIsDir = CDirHelper::IsDirectory(findData);
			bool bIsFile= CDirHelper::IsFile(findData);
			if( bIsDir || bIsFile)
			{
				if(!(t.*pProcessFunc)(sDir,findData))
				{
					break;
				}
				if(bIsDir && bRecursive)
				{
					WalkDirTree(sDir+findData.cFileName+"\\",
								pProcessFunc,t, bRecursive);
				}
			}
		}while(FindNextFile(hFind,&findData));
		::FindClose(hFind);
	}
};


#endif