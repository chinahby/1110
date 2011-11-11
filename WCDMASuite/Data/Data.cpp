#include "stdafx.h"
#include "data.h"
#include "../GlobalDataCenter.h"
#include "../Util.h"
#include "../CodingConv.h"
#include <Afxmt.h>
#include "../SunnyCatDlg.h"

static CCriticalSection g_CS;

sqlite3* Data::m_pDB = NULL;

Data::Data()
{

}

Data::~Data()
{

}

sqlite3* const Data::OpenGlobalDB(void)
{
    static sqlite3* db = 0;
	CString strDB;
	CString strProduct = GetProduct();
//	if(strProduct == "-p")
//		strDB = "SC700.DB";
//	else if(strProduct == "-u")
		strDB = "M500EUSB.DB";
//	else if(strProduct == "-c")
//		strDB = "M500ECOM.DB";

	CString strapplPath = GetAppPath();
	CString strfileName(strDB);//"SC700.DB"
	CString tempPath = strapplPath+strfileName;
	const TCHAR * pathOld = (LPCTSTR)(tempPath);
	CFileFind   finder;   
	if(!finder.FindFile(pathOld))   
	{   
		GMessageBox(::LoadStringEx(IDS_NO_DATABASE));  
		//当没有数据库时强制退出，目前没有好的办法退出，暂时使用ExitProcess。
		ExitProcess(0);
	}   
	
	char pathNew[1024] = {0};
	std::string str = ToMultiBytes(pathOld,_tcslen(pathOld));
	CCodingConv::GB2312_2_UTF8(pathNew, 1024, str.c_str(), 0);
	int rc = sqlite3_open(pathNew, &m_pDB);
	if (rc)
	{
		sqlite3_close(m_pDB);
		return m_pDB;
	}
   
    return m_pDB;
}

int Data::CloseGlobalDB()
{
	return sqlite3_close(m_pDB);
}

sqlite3* const Data::GetClobalDB()
{
	return m_pDB;
}

void Data::SetTable(CString tableName)
{
	m_tableName = tableName;
}

bool Data::exeBlobCommand(CString const cmd,CString const strBlob)
{
	g_CS.Lock();

	sqlite3_stmt      *stat = 0;
	std::string str = ToMultiBytes(cmd,cmd.GetLength());
	int ret = sqlite3_prepare(m_pDB, str.c_str(), -1, &stat, 0);
	if (ret != SQLITE_OK)
	{
		ret  = false;
	}
	else
	{
		LPTSTR pTemp = const_cast<LPTSTR>((LPCTSTR(strBlob)));
		
		void  *pBlob = reinterpret_cast<void*>(pTemp);
		int n = strBlob.GetLength()*2;
		ret = sqlite3_bind_blob(stat, 1, pBlob, strBlob.GetLength()*2, NULL);
		if (ret != SQLITE_OK)
		{
			ret =  false;
		}		
		else
		{
			ret = sqlite3_step(stat);
			if (ret != SQLITE_DONE)
			{
				ret = false;
			}
			ret = sqlite3_finalize(stat);			
		}	
		
	}

	g_CS.Unlock();

	return ret == SQLITE_OK;
}

bool const Data:: execCommand(CString const cmd) const
{
	g_CS.Lock();

	bool result = true;
	char* errorMessage = 0;
	std::string str = ToMultiBytes(cmd,cmd.GetLength());
	int rc = sqlite3_exec(m_pDB, str.c_str(), 0, 0, &errorMessage);
	if (rc != SQLITE_OK)
	{
//		AfxMessageBox(errorMessage);
		//GMessageBox(errorMessage);
		result = false;
		
		//log_(errorMessage);
		//indication_(errorMessage);
		//result = false;
	}

	g_CS.Unlock();

	return result;
}


BOOL Data::getDatasByFilter(CString const filter, void* pData, int const maxCount) const
{	
	//SELECT * FROM [index] ORDER BY indexid DESC limit 0,10;
	g_CS.Lock();

	BOOL bRet = TRUE;
	CString cmd = "SELECT";
	cmd += " * FROM [";
	cmd += m_tableName;
	cmd += "]";
	if (filter != "")
	{
		cmd += " WHERE ";
		cmd += filter;
	}
	if (maxCount != -1)
	{
		cmd += " ORDER BY ClassID DESC limit 0, " + Util::intToString(maxCount);
		
	}
	
	char* errorMessage = 0;
	std::string str = ToMultiBytes(cmd,cmd.GetLength());
	int rc = sqlite3_exec(m_pDB, str.c_str(), rowProcess, pData ,&errorMessage);
	if (rc != SQLITE_OK)
	{
		
		
//		AfxMessageBox(errorMessage);
		//GMessageBox(errorMessage);
		bRet = FALSE;
		
		//indication_(errorMessage);
		//throw false;
	}

	g_CS.Unlock();
	
	return bRet;
}

BOOL Data::getDatasOrderByTime(CString const filter, void* pData, int const maxCount) const
{	
	//SELECT * FROM [index] ORDER BY indexid DESC limit 0,10;
	g_CS.Lock();

	BOOL bRet = TRUE;
	CString cmd = "SELECT";
	cmd += " * FROM [";
	cmd += m_tableName;
	cmd += "]";
	if (filter != "")
	{
		cmd += filter;
	}
	
	char* errorMessage = 0;
	std::string str = ToMultiBytes(cmd,cmd.GetLength());
	int rc = sqlite3_exec(m_pDB, str.c_str(), rowProcess, pData ,&errorMessage);
	if (rc != SQLITE_OK)
	{
		
		
//		AfxMessageBox(errorMessage);
		//GMessageBox(errorMessage);
		bRet = FALSE;
		
		//indication_(errorMessage);
		//throw false;
	}

	g_CS.Unlock();
	
	return bRet;
}

bool const Data::removeDatasByFilter(CString const filter) const
{
	CString cmd = "DELETE FROM [";
	cmd += m_tableName;
	cmd += "]";
	if (filter != "")
	{
		cmd += " WHERE ";
		cmd += filter;
	}
	return execCommand(cmd);
}

CString Data::GettableName(void) const
{
	return m_tableName;
}

int Data:: rowProcess(void* parameter, int argc, char** argv, char** columnName)
{
	g_CS.Lock();

	((Data*)parameter)->modifyFieldByDB(argc, argv, columnName);
	
	g_CS.Unlock();
	
	return 0;
}

int Data::getCount(void* parameter, int argc, char** argv, char** columnName) const
{
	int* result = reinterpret_cast< int*>(parameter);
	// HXB:int* result = reinterpret_cast<unsigned int*>(parameter);
	*result = atoi(argv[Util::getIndexByName(argc, columnName, "recordCount")]);
	return 0;
}

int Data::GetMaxID(CString strSQL)
{
	g_CS.Lock();

	char *szErrMsg = 0;
	char ** resOfCnt;
	int resRow, resCol; // rows and columns of the result.
	int ret;
	std::string str = ToMultiBytes(strSQL,strSQL.GetLength());
	int rc = sqlite3_get_table(m_pDB, str.c_str(), &resOfCnt, &resRow, &resCol, &szErrMsg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error:%s\n", szErrMsg);
		sqlite3_free(szErrMsg);
		ret = -1;
	} 
	else 
	{
		if(resOfCnt[1] == NULL)
                {
			ret = -1;
                }
		else
		{
			ret = atoi(resOfCnt[1]);
			sqlite3_free_table(resOfCnt); // 释放结果占用的资源
		}
				
	}
	
	g_CS.Unlock();
	
	return ret;
}

int Data::GetColCount(CString strSQL)
{
	g_CS.Lock();

	char *szErrMsg = 0;
	char ** resOfCnt;
	int resRow, resCol; // rows and columns of the result.
	int ret;
	std::string str = ToMultiBytes(strSQL,strSQL.GetLength());
	int rc = sqlite3_get_table(m_pDB, str.c_str(), &resOfCnt, &resRow, &resCol, &szErrMsg);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "SQL error:%s\n", szErrMsg);
		sqlite3_free(szErrMsg);
		ret = -1;
	} 
	else 
	{
		if(resOfCnt[1] == NULL)
                {
			ret = -1;
                }
		else
		{
			ret = atoi(resOfCnt[1]);
			sqlite3_free_table(resOfCnt); // 释放结果占用的资源
		}
		
	}
	
	g_CS.Unlock();

	return ret;
}

CString Data::ChangeStrToStr(CString strValue)
{
	CString str;
	
	str = strValue;
	str.Replace(TEXT("'"),TEXT("''"));

	return str;
//	if(str.Find("'") == -1)
//		return str;
	
}