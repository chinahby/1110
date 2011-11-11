#include "stdafx.h"
#include "PhonebookPC.h"
#include "../CriticalSecMon.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//CRITICAL_SECTION		g_PBCS;//临界区保护
CCriticalSection g_PBCS;
//CPhonebookPCClass
CPhonebookPCClass::CPhonebookPCClass()
{
	SetTable("SC700_PBClass");
}

CPhonebookPCClass::~CPhonebookPCClass()
{
	ClearVector();
}

bool CPhonebookPCClass::Add(const TPBClass& item)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();
	
	TPBClass* pItem = new TPBClass(item);
	m_Data.push_back(pItem);
	
	TPBClass PBClassItem;
	PBClassItem = item;
	ChangeItem(PBClassItem);

	CString strSQL = "INSERT INTO ";

	strSQL += GettableName();
	strSQL += "(PBClassID, PBParentID, PBClassName, PBClassVoice)Values(";
	strSQL += "NULL,";	
	strSQL += Util::intToString(item.nParentID);
	strSQL += ", '";
	strSQL += PBClassItem.strClassName;
	strSQL += "', '";
	strSQL += PBClassItem.strVoicePath;	
	strSQL += "') ";

	bool  bRet = (BOOL)execCommand(strSQL);

	g_PBCS.Unlock();

	return bRet;
}

bool CPhonebookPCClass::execCommand_(CString cmd)
{
	return execCommand(cmd);
}

int CPhonebookPCClass::FindIndexByID(size_t id)const
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	int nRet = -1;
	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Data[i]->nID == id)
		{
			nRet = i;
			break;
		}
	}
	//nRet = -1;

	g_PBCS.Unlock();

	return nRet;
}



bool CPhonebookPCClass::Delete(size_t nID)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();
	int nIndex = FindIndexByID(nID);
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < Size());

	delete m_Data[nIndex];
	m_Data.erase(m_Data.begin()+nIndex);

	CString strSQL;
	
	strSQL.Format(TEXT("PBClassID = %d"),nID);
	bool bRet = removeDatasByFilter(strSQL);
	
	g_PBCS.Unlock();
	
	return bRet;
}



bool CPhonebookPCClass::Modify(const TPBClass& item)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();
	
	TPBClass PBClassItem;
	PBClassItem = item;
	ChangeItem(PBClassItem);

	CString strSQL = "UPDATE ";

	strSQL += GettableName();
	strSQL += " SET PBClassName = '";
	strSQL += PBClassItem.strClassName;
	strSQL += "' ,";
	strSQL += " PBClassVoice = '";
	strSQL += PBClassItem.strVoicePath;
	strSQL += "' WHERE PBClassID = ";
	strSQL += Util::intToString(item.nID);
	int id = FindIndexByID(item.nID);
	m_Data[id]->strClassName = item.strClassName;
	m_Data[id]->strVoicePath = item.strVoicePath;
	bool bRet = execCommand(strSQL);

	g_PBCS.Unlock();
	
	return bRet;
}

void CPhonebookPCClass::modifyFieldByDB(int argc, char** argv, char** columnName)
{
	g_PBCS.Lock();

	TPBClass* pTPBClass = new TPBClass();

	pTPBClass->nID = atoi(argv[Util::getIndexByName(argc, columnName, "PBClassID")]);
	pTPBClass->nParentID = atoi(argv[Util::getIndexByName(argc, columnName, "PBParentID")]);
	pTPBClass->strClassName = argv[Util::getIndexByName(argc, columnName, "PBClassName")];
	pTPBClass->strVoicePath = argv[Util::getIndexByName(argc, columnName, "PBClassVoice")];
	
	m_Data.push_back(pTPBClass);

	g_PBCS.Unlock();
}

void CPhonebookPCClass::ClearVector()
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	for(size_t i=0; i<Size(); ++i)
		delete m_Data[i];

	m_Data.clear();

	g_PBCS.Unlock();
}

size_t CPhonebookPCClass::Size() const
{
	return m_Data.size();
}

const TPBClass& CPhonebookPCClass::GetAt(size_t index)const
{
	return * m_Data[index];
}

BOOL CPhonebookPCClass::GetAllRecords()
{
	g_PBCS.Lock();

	CString strSQL;
	strSQL.Empty();
	BOOL bRet = getDatasByFilter(strSQL,this);

	g_PBCS.Unlock();
	return bRet;
}

void CPhonebookPCClass::ChangeItem(TPBClass& item)
{
	item.strClassName = ChangeStrToStr(item.strClassName);
	item.strVoicePath = ChangeStrToStr(item.strVoicePath);
}

int CPhonebookPCClass::FindIDByName(CString strName)
{
	g_PBCS.Lock();

	int nCount = Size();
	int i = 0;
	int nRet = -1;

	for(i=0 ; i < nCount; i++)
	{
		if(m_Data[i]->strClassName  == strName)
		{
			nRet = m_Data[i]->nID;
			break;
		}
	}

	g_PBCS.Unlock();

	return nRet;
}

CString CPhonebookPCClass::FindNameByID(int id)
{
	g_PBCS.Lock();
	
	int nCount = Size();
	int i = 0;

	CString result = "";
	for(i=0 ; i < nCount; i++)
	{
		if(m_Data[i]->nID  == id)
		{
			result = m_Data[i]->strClassName;
			break;
		}
	}
	
	g_PBCS.Unlock();
	
	return result;
}
//CPhonebookPC
CPhonebookPC::CPhonebookPC()
{
	SetTable("SC700_PhoneBook");
}

CPhonebookPC::~CPhonebookPC()
{
	ClearVector();
//	sqlite3_close(m_pDB);
}

void CPhonebookPC::Clear()
{
	//CCriticalMonitor mon(g_PBCS);

	g_PBCS.Lock();
	
	ClearVector();

	CString strSQL;
	CString str;
	str = ChangeStrToStr(m_strTypeName);

	strSQL.Format(TEXT("TypeName = '%s'"),str);
	removeDatasByFilter(strSQL);

	g_PBCS.Unlock();
}

void CPhonebookPC::ClearVector()
{
	for(size_t i=0; i<m_Data.size(); ++i)
		delete m_Data[i];

	m_Data.clear();
}

bool	CPhonebookPC::Add(const TPhoneBookItemEx& item)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();
	
	TPhoneBookItemEx* pItem = new TPhoneBookItemEx(item);
	m_Data.push_back(pItem);
	
	Insert(item);

	g_PBCS.Unlock();

	return true;
}

bool	CPhonebookPC::Delete(size_t nID)
{

	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	int nIndex = FindIndexByID(nID);
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < Size());

	delete m_Data[nIndex];
	m_Data.erase(m_Data.begin()+nIndex);
	
	CString strSQL;
	strSQL = "PhoneID = " + Util::intToString(nID);
	//strSQL.Format("PhoneID = '%d'",nID);
	bool bRet = removeDatasByFilter(strSQL);

	g_PBCS.Unlock();

	return bRet;
}

int CPhonebookPC::FindIndexByID(size_t id)const
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	int nRet = -1;
	int nSize = Size();
	for(size_t i=0; i<nSize; ++i)
	{
		if(m_Data[i]->nID == id)
		{
			nRet = i;
			break;
		}
	}
	//nRet = -1;

	g_PBCS.Unlock();

	return nRet;
}


bool	CPhonebookPC::Modify(const TPhoneBookItemEx& item)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();
	
	CString strSQL = "UPDATE ";
	TPhoneBookItemEx PBItem;

	PBItem = item;
	ChangeItem(PBItem);

	strSQL += GettableName();
	strSQL += " SET TypeName = '";
	strSQL += PBItem.strTypeName;
	strSQL += "',ParentID = ";
	strSQL += Util::intToString(item.nParentID);
	strSQL += ",Name = '";
	strSQL += PBItem.strName;
	strSQL += "',Business = '";
	strSQL += PBItem.strBusiness;
	strSQL += "',Email = '";
	strSQL += PBItem.strEmail;
	strSQL += "',Country = '";
	strSQL += PBItem.strCountry;
	strSQL += "',HomePhone = '";
	strSQL += PBItem.strHomePhone;
	strSQL += "',MobilePhone = '";
	strSQL += PBItem.strMobilePhone;
	strSQL += "',Fax = '";
	strSQL += PBItem.strFax;
	strSQL += "',Office = '";
	strSQL += PBItem.strOffice;
	strSQL += "',OfficePhone = '";
	strSQL += PBItem.strOfficePhone;

	strSQL += "' WHERE PhoneID = ";
	strSQL += Util::intToString(item.nID);

	bool bRet = execCommand(strSQL);

	g_PBCS.Unlock();

	return bRet;
}

const	TPhoneBookItemEx& CPhonebookPC::GetAt(size_t index)const
{
	return * m_Data[index];
}

size_t	CPhonebookPC::Size()const
{
	return m_Data.size();
}

struct IsPBExItemEqual//判断两个phonebook item 是否相等
{
	const TPhoneBookItem& m_item;
	const TPhoneBookItemEx* m_ItemEx;
public:
	IsPBExItemEqual(const TPhoneBookItem& item)
		:m_item(item)
	{
		m_ItemEx = (const TPhoneBookItemEx*)(&m_item);
	}
	bool operator()(const TPhoneBookItemEx* item)const
	{
		bool bOK =  m_item.strName == item->strName && m_item.strMobilePhone == item->strMobilePhone;
		if(bOK && m_ItemEx)
		{
			bOK =	m_ItemEx->strHomePhone == item->strHomePhone &&
					m_ItemEx->strOffice == item->strOffice &&
					m_ItemEx->strEmail == item->strEmail;
		}

		return bOK;
	}
};
bool	CPhonebookPC::Find(const TPhoneBookItemEx& item, size_t* pIndex)
{
	std::vector<TPhoneBookItemEx*>::const_iterator iFind = 
		std::find_if(m_Data.begin(), m_Data.end(), IsPBExItemEqual(item));
	if(iFind == m_Data.end())
		return false;
	if(pIndex)
		*pIndex = iFind-m_Data.begin();

	return true;
}

struct PBItemEx_NameOrderUp
{
	const operator()(const TPhoneBookItemEx* itemA, TPhoneBookItemEx* itemB)const
	{
		return itemA->strName < itemB->strName;
	}
};

struct PBItemEx_NameOrderDown
{
	const operator()(const TPhoneBookItemEx* itemA, TPhoneBookItemEx* itemB)const
	{
		return itemA->strName > itemB->strName;
	}
};

void	CPhonebookPC::SortByName(bool bUpOrder)
{
	if(bUpOrder)
		std::sort(m_Data.begin(), m_Data.end(), PBItemEx_NameOrderUp());
	else
		std::sort(m_Data.begin(), m_Data.end(), PBItemEx_NameOrderDown());
}

size_t  CPhonebookPC::MaxStorage()const
{
	return 9999;
}

BOOL CPhonebookPC::GetRecordsByNumber(CString number)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();
	
	CString strSQL;
	CString strOther;
	CString str;
	str = ChangeStrToStr(number);
	strSQL.Format(TEXT(" HomePhone like '%%%s%%'"),str); 
	strOther.Format(TEXT(" OR MobilePhone like '%%%s%%'"),str);
	strSQL += strOther;
	strOther.Format(TEXT(" OR OfficePhone like '%%%s%%'"),str);
	strSQL += strOther;
	strOther.Format(TEXT(" OR Fax like '%%%s%%'"),str);
	strSQL += strOther;
	strOther.Format(TEXT(" OR Name like '%%%s%%'"),str);
	strSQL += strOther;
	
	BOOL bRet = getDatasByFilter(strSQL,this);

	g_PBCS.Unlock();

	return bRet;
}

BOOL CPhonebookPC::GetRecordsByName(CString name)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	CString strSQL;
	CString str;
	str = ChangeStrToStr(name);
	strSQL = " Name = '" +  str; 
	strSQL += "' ";
	BOOL bRet = getDatasByFilter(strSQL,this);

	g_PBCS.Unlock();

	return bRet;
}

BOOL CPhonebookPC::GetRecordsByTypeName(CString strTypeName)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	m_strTypeName = strTypeName;

	CString strSQL;
	CString str = ChangeStrToStr(strTypeName);
	strSQL.Format(TEXT("TypeName = '%s'"),str);
	BOOL bRet = getDatasByFilter(strSQL,this);

	g_PBCS.Unlock();

	return bRet;
}

BOOL CPhonebookPC::ClearCurrClassCard(int nParentID)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	ClearVector();

	CString strSQL;

	strSQL.Format(TEXT("ParentID = '%d'"),nParentID);
	removeDatasByFilter(strSQL);

	g_PBCS.Unlock();

	return 0;
}
BOOL CPhonebookPC::GetRecordsByTypeName(int nParentID)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	CString strSQL;
	//strSQL.Format("ParentID = '%d'",nParentID);
	strSQL = "ParentID = ";
	strSQL += Util::intToString(nParentID);
	BOOL bRet = getDatasByFilter(strSQL,this);

	g_PBCS.Unlock();

	return bRet;
}

void CPhonebookPC:: modifyFieldByDB(int argc, char** argv, char** columnName)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	TPhoneBookItemEx* pPhoneBookItemEx = new TPhoneBookItemEx();

	pPhoneBookItemEx->nID = atoi(argv[Util::getIndexByName(argc, columnName, "PhoneID")]);
	pPhoneBookItemEx->strTypeName = argv[Util::getIndexByName(argc, columnName, "TypeName")];
	pPhoneBookItemEx->nParentID = atoi(argv[Util::getIndexByName(argc, columnName, "ParentID")]);
	pPhoneBookItemEx->strName = argv[Util::getIndexByName(argc, columnName, "Name")];
	pPhoneBookItemEx->strBusiness = argv[Util::getIndexByName(argc, columnName, "Business")];
	pPhoneBookItemEx->strEmail = argv[Util::getIndexByName(argc, columnName, "Email")];
	pPhoneBookItemEx->strCountry = argv[Util::getIndexByName(argc, columnName, "Country")];
	pPhoneBookItemEx->strHomePhone = argv[Util::getIndexByName(argc, columnName, "HomePhone")];
	pPhoneBookItemEx->strMobilePhone = argv[Util::getIndexByName(argc, columnName, "MobilePhone")];
	pPhoneBookItemEx->strFax = argv[Util::getIndexByName(argc, columnName, "Fax")];
	pPhoneBookItemEx->strOffice = argv[Util::getIndexByName(argc, columnName, "Office")];
	pPhoneBookItemEx->strOfficePhone = argv[Util::getIndexByName(argc, columnName, "OfficePhone")];

	m_Data.push_back(pPhoneBookItemEx);	

	g_PBCS.Unlock();
}

BOOL CPhonebookPC::Insert(const TPhoneBookItemEx& item)
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();

	TPhoneBookItemEx PBItem; 

	PBItem = item;
	ChangeItem(PBItem);

	CString strSQL = "INSERT INTO ";

	strSQL += GettableName();
	strSQL += "(PhoneID,TypeName,ParentID,Name,Business,Email,Country,HomePhone,MobilePhone,Fax,Office,OfficePhone)Values(";
	strSQL += "NULL",
	strSQL += ",'",
	strSQL += PBItem.strTypeName;
	strSQL += "',";
	strSQL += Util::intToString(item.nParentID);
	strSQL += ",'";
	strSQL += PBItem.strName;
	strSQL += "','";
	strSQL += PBItem.strBusiness;
	strSQL += "','";
	strSQL += PBItem.strEmail;
	strSQL += "','";
	strSQL += PBItem.strCountry;
	strSQL += "','";
	strSQL += PBItem.strHomePhone;
	strSQL += "','";
	strSQL += PBItem.strMobilePhone;
	strSQL += "','";
	strSQL += PBItem.strFax;
	strSQL += "','";
	strSQL += PBItem.strOffice;
	strSQL += "','";
	strSQL += PBItem.strOfficePhone;
	strSQL += "')";

	BOOL bRet = execCommand(strSQL);

	g_PBCS.Unlock();

	return bRet;
}

BOOL CPhonebookPC::GetAllRecords()
{
	//CCriticalMonitor mon(g_PBCS);
	g_PBCS.Lock();
	
	CString strSQL;

	strSQL.Empty();
	BOOL bRet = getDatasByFilter(strSQL,this);
	
	g_PBCS.Unlock();
	
	return bRet;
}

BOOL CPhonebookPC::IsExitRecord(const TPhoneBookItemEx& item,BOOL bIsOutLook)
{
	g_PBCS.Lock();

	CString strSQL;
	CString strOther;
	TPhoneBookItemEx PBItem;
	PBItem = item;
	ChangeItem(PBItem);
	strSQL.Empty();
	if(!bIsOutLook)
	{
		strSQL.Format(TEXT("TypeName = '%s'"),PBItem.strTypeName);
		strSQL += strOther;
		strOther.Format(TEXT(" And Name = '%s'"),PBItem.strName);
	}
	else
	{
		strOther.Format(TEXT("Name = '%s'"),PBItem.strName);
	strSQL += strOther;
		strOther.Format(TEXT(" And ParentID = %ld"),PBItem.nParentID);
	}
	strSQL += strOther;
	strOther.Format(TEXT(" And Business = '%s'"),PBItem.strBusiness);
	strSQL += strOther;
	strOther.Format(TEXT(" And Email = '%s'"),PBItem.strEmail);
	strSQL += strOther;
	strOther.Format(TEXT(" And Country = '%s'"),PBItem.strCountry);
	strSQL += strOther;
	strOther.Format(TEXT(" And HomePhone = '%s'"),PBItem.strHomePhone);
	strSQL += strOther;
	strOther.Format(TEXT(" And MobilePhone = '%s'"),PBItem.strMobilePhone);
	strSQL += strOther;
	strOther.Format(TEXT(" And Fax = '%s'"),PBItem.strFax);
	strSQL += strOther;
	strOther.Format(TEXT(" And Office = '%s'"),PBItem.strOffice);
	strSQL += strOther;
	strOther.Format(TEXT(" And OfficePhone = '%s'"),PBItem.strOfficePhone);
	strSQL += strOther;

	if(!bIsOutLook)
	{
		strOther.Format(TEXT(" And PhoneID <> %ld"),PBItem.nID);
		strSQL += strOther;
	}

	BOOL bRet = getDatasByFilter(strSQL,this);

	if(Size() >0)
		bRet = TRUE;
	else
		bRet = FALSE;
	
	g_PBCS.Unlock();

	return bRet;
}

void CPhonebookPC::ChangeItem(TPhoneBookItemEx& item)
{
	item.strTypeName = ChangeStrToStr(item.strTypeName);
	item.strName = ChangeStrToStr(item.strName);
	item.strBusiness = ChangeStrToStr(item.strBusiness);
	item.strCountry = ChangeStrToStr(item.strCountry);
	item.strEmail = ChangeStrToStr(item.strEmail);
	item.strFax = ChangeStrToStr(item.strFax);
	item.strHomePhone = ChangeStrToStr(item.strHomePhone);
	item.strMobilePhone = ChangeStrToStr(item.strMobilePhone);
	item.strOffice = ChangeStrToStr(item.strOffice);
	item.strOfficePhone = ChangeStrToStr(item.strOfficePhone);
}
