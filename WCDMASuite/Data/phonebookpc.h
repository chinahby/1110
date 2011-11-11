#ifndef AFX_PHONEBOOKPC_H__9F171FCF_2CA3_46D4_80BE_CDC8F2D8FFB5__INCLUDED_
#define AFX_PHONEBOOKPC_H__9F171FCF_2CA3_46D4_80BE_CDC8F2D8FFB5__INCLUDED_

#include "PhoneBook.h"
#include "Data.h"
#include <vector>

struct TPBClass
{
	int nID;	//名片夹ID,由sqlite3自动分配
	int nParentID;	//名片夹父ID，保留，将来扩展
	CString strClassName;	//名片夹名字
	CString strVoicePath;	//铃声路径
};

class CPhonebookPCClass : public Data
{
public:
	CPhonebookPCClass();
	~CPhonebookPCClass();
public:
	bool	Add(const TPBClass& item);	//增加一个新名片夹
	bool	Delete(size_t nID);				//根据ID删除一个名片夹	
	bool	Modify(const TPBClass& item);	//修改一个名片夹

	virtual const	TPBClass& GetAt(size_t index)const; //得到一条信息
	virtual size_t	Size()const;
	bool execCommand_(CString cmd);
	void ClearVector(); 
	BOOL GetAllRecords();	//得到全部名片夹信息
	int FindIndexByID(size_t id)const;
	int FindIDByName(CString strName);
	CString FindNameByID(int id);
private:
	virtual void  modifyFieldByDB(int argc, char** argv, char** columnName);
	void ChangeItem(TPBClass& item);
	
private:
	TPBClass m_PBClass;
	sqlite3* m_pDB;
	std::vector<TPBClass*> m_Data;
};	
class CPhonebookPC : public Data,public CPhonebook   
{
public:
	CPhonebookPC();
	virtual ~CPhonebookPC();
	
public:
	virtual bool	Add(const TPhoneBookItemEx& item);	//增加一条联系人信息
	virtual bool	Delete(size_t nID);				//根据ID删除一条联系人信息	
	virtual bool	Modify(const TPhoneBookItemEx& item);	//修改一条信息
	virtual const	TPhoneBookItemEx& GetAt(size_t index)const; //得到一条信息
	virtual size_t	Size()const;						//返回联系人个数

	virtual bool	Find(const TPhoneBookItemEx& item, size_t* pIndex);
	virtual void	SortByName(bool bUpOrder);
	virtual size_t  MaxStorage()const;
	BOOL GetRecordsByTypeName(CString strTypeName);	//根据名片夹名得到当前名片夹下全部名片信息
	BOOL GetRecordsByNumber(CString number);
	BOOL GetRecordsByTypeName(int nParentID);	//得到所有名片夹下全部名片信息
	BOOL GetRecordsByName(CString name);
	BOOL GetAllRecords();		//得到所有电话本信息
	void Clear();	//清空当前名片夹下所有联系人信息
	BOOL ClearCurrClassCard(int nParentID);
	void ClearVector();
	int FindIndexByID(size_t id)const;
	BOOL IsExitRecord(const TPhoneBookItemEx& item,BOOL bIsOutLook);	//判断是否有重复记录
private:
	virtual void  modifyFieldByDB(int argc, char** argv, char** columnName);
	BOOL Insert(const TPhoneBookItemEx& item);	//插入一条联系人信息
	void ChangeItem(TPhoneBookItemEx& item);
	
private:
	CPhonebookPC(const CPhonebookPC&);
	CPhonebookPC& operator=(const CPhonebookPC&);
private:
	std::vector<TPhoneBookItemEx*> m_Data;
	CString m_strTypeName;

	sqlite3* m_pDB;
};

/*
CREATE TABLE SC700_PBClass(PBClassID integer PRIMARY KEY,
					PBParentID int default(0),
					PBClassName char(50) default(NULL),
					PBClassVoice char(256) default(NULL))
*/

/*
CREATE TABLE SC700_PhoneBook(PhoneID integer PRIMARY KEY,
							 typename char(30) default(NULL),
							 ParentID int default(1),
							 Name char(50) default(NULL),
							 Business char(30) default(NULL),
							 Email char(50) default(NULL),
							 Country char(50) default(NULL),
							 HomePhone char(20) default(NULL),
							 MobilePhone char(20) default(NULL),
							 Fax char(20) default(NULL),
							 Office char(50) default(NULL),
							 OfficePhone char(20) default(NULL))
*/
#endif
