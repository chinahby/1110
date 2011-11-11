#include "stdafx.h"
#include "Execute.h"
#include "DirWalker.h"
#include <fstream>

CString GetAppPath();

class CFileCopier
{
public:
	CFileCopier(const string& strDestDir, const string& strLog)
		:m_strDestDir(strDestDir)
	{
		string strLogFile = strDestDir+strLog;
		m_LogFile.open(strLogFile.c_str());
	}
public:
	bool OnFileList(const string& strPath,const WIN32_FIND_DATA& fData)
	{
		if(CDirHelper::IsFile(fData))
		{
			string strSrc = strPath+fData.cFileName;
			string strDst = m_strDestDir+fData.cFileName;
			CopyFile(strSrc.c_str(), strDst.c_str(),false);
			SetFileAttributes(strDst.c_str(), FILE_ATTRIBUTE_NORMAL);

			m_LogFile<<fData.cFileName<<endl;
		}
		else
		{
			CreateDirectory((m_strDestDir+fData.cFileName).c_str(), NULL);
		}
		return true;
	}
private:
	string m_strDestDir;
	ofstream m_LogFile;
};

bool DirExisting(const CString& strDir)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = ::FindFirstFile(strDir,&findData);
	if(hFind == INVALID_HANDLE_VALUE)return false;
	::FindClose(hFind);

	return true;
}

bool FileExisting(const CString& strFile)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = ::FindFirstFile(strFile,&findData);
	if(hFind == INVALID_HANDLE_VALUE)return false;
	::FindClose(hFind);

	return true;
}

void CopyFilesInDir(const CString& strPathFrom, const CString& strDirTo)
{
	CDirWalker<CFileCopier> dirWalker;
	CFileCopier copier((LPCSTR)strDirTo, "copy-resource.log");
	dirWalker.WalkDirTree((LPCSTR)strPathFrom, copier.OnFileList, copier, false);
}

void RemoveFileInDir(const CString& strPath, const CString& strFile)
{
	DeleteFile(strPath+strFile);
}


/**
*	拷贝..\oem\language到 strDestDir\resource，
*	并纪录拷贝的文件列表为strDestDir\copy-resource.log
*/
bool CopyResourceDir(const CString& strLanguage, const CString& strDestDir)
{
	CString strPathFrom = GetAppPath();
	strPathFrom += "..\\OEM\\";
	strPathFrom += strLanguage;

	CString strDirTo = strDestDir;
	strDirTo += "\\Resource\\";

	CreateDirectory(strDirTo,NULL);
	CopyFilesInDir(strPathFrom, strDirTo);

	return true;
}

/**
*	根据拷贝的文件列表 strDestDir\copy-resource.log中的纪录
*	删除strDestDir\中的文件，如果删除后的strDestDir 目录为空
*	则删除该目录
*/
bool RemoveResourceDir(const CString& strDestDir)
{
	CString strLog = strDestDir+"Resource\\copy-resource.log";
	{
		ifstream logFile(strLog);
		if(logFile.is_open())
		{
			char buf[256];
			while(!logFile.eof())
			{
				memset(buf,0, sizeof(buf));
				logFile.getline(buf,sizeof(buf));
				DeleteFile(strDestDir+"Resource\\"+buf);
			}
		}
		logFile.close();
	}

	DeleteFile(strLog);
	RemoveDirectory(strDestDir+"Resource");
	return true;
}



