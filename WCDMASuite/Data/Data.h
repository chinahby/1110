#ifndef _DATA_H_
#define _DATA_H_

#include <vector>
#include "sqlite3.h"


typedef char* DataManipulatorError;

class Util
{
public:
    static CString const intToString(int const v)
    {
        char result[32] = {0};
        sprintf(result, "%d", v);
        return result;
    }
    static int const getIndexByName(int const nameCount, char const* const names[], char const* const name)
    {
        int result = -1;
        for (int i = 0; i < nameCount; ++i)
        {
            if (strcmp(names[i], name) == 0)
            {
                result = i;
                break;
            }
        }
        return result;
    }
    static bool const stringToBool(std::string const value)
    {
        return false;
    }
};


class Data
{
public:
	Data();
	virtual ~Data();		
	static sqlite3* const OpenGlobalDB(void);
	static int CloseGlobalDB();
	static sqlite3* const GetClobalDB();
public:
			
	void SetTable(CString tableName);	
    bool const execCommand(CString const cmd) const;	
	BOOL getDatasByFilter(CString const filter, void* pData, int const maxCount = -1) const;  	
	BOOL getDatasOrderByTime(CString const filter, void* pData, int const maxCount) const;
	bool const removeDatasByFilter(CString const filter) const;		
	CString GettableName(void) const;
	int GetMaxID(CString strSQL);
	int GetColCount(CString strSQL);
	CString ChangeStrToStr(CString strValue);
	bool exeBlobCommand(CString const cmd,CString const strBlob);
private:
	virtual void  modifyFieldByDB(int argc, char** argv, char** columnName) = 0;
	void (*indication_)(DataManipulatorError& e);
	static int rowProcess(void* parameter, int argc, char** argv, char** columnName);
	int getCount(void* parameter, int argc, char** argv, char** columnName) const;
	
	
private:	
	CString m_tableName;
	static sqlite3* m_pDB;
};

#endif
