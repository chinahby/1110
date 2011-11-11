#include "stdafx.h"
#include "PhonebookEmbed.h"
#include "../ThreadLoadPB.h"
#include "../MobileManager.h"
#include "../SunnyCatDlg.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPhonebookEmbed::CPhonebookEmbed(const CString&  type)
:m_Type(type), m_MinID(1), m_MaxID(1), m_bLoaded(false),m_bFull(false)
{

}

CPhonebookEmbed::~CPhonebookEmbed()
{
	Clean();
}

void CPhonebookEmbed::Clean()
{
	for(size_t i=0; i<m_Data.size(); ++i)
	{
		delete m_Data[i];
	}

	m_Data.clear();
}

void CPhonebookEmbed::AddItem(const TPhoneBookItem& item, int id)
{
	TPhoneBookItemEmbed* pNew = new TPhoneBookItemEmbed;
	pNew->strName		= item.strName;
	pNew->strMobilePhone	= item.strMobilePhone;
	pNew->ID		= id;

	m_Data.push_back(pNew);
}


void CPhonebookEmbed::Load()
{
	if(m_bLoaded)return;

//	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.GetConnectionStat() != CONNECT_CONTROL_NOT_CONNECT)return;

	//m_bLoaded = true;	

	CThreadLoadPB thread(((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager,*this);
	CDlgProgressEx dlg(thread, NULL, false);
	dlg.DoModal();

	AfxGetApp()->m_pMainWnd->PostMessage(WM_PHONEBOOK_CHANGED,0,0);
}


//找到一个空位置来存放新的条目
int  CPhonebookEmbed::FreePlace()const
{
	int nSize = m_MaxID - m_MinID+1;
	int* IDArray = new int[nSize];
	for(int i=0; i<nSize; ++i) IDArray[i]=0;
	for(i=0; i<m_Data.size(); ++i) IDArray[m_Data[i]->ID-m_MinID] = 1;

	int nID =-1;
	for(i=0; i<nSize; ++i)
	{
		if(!IDArray[i])
		{
			nID = m_MinID+i;
			break;
		}
	}

	delete[] IDArray;

	return nID;
}

bool	CPhonebookEmbed::Add(const TPhoneBookItem& item)
{
//	Load();

	int nID = FreePlace();
	if(nID<m_MinID)
	{
		m_bFull = true;
		return false;
	}
	m_bFull = false;

	TPhoneBookItem itemNew;
	itemNew.strName = item.strName;
	itemNew.strMobilePhone = item.strMobilePhone;
	const TPhoneBookItemEx* itemEx = (TPhoneBookItemEx*)(&item);
	if(itemEx)
	{
		if(itemNew.strMobilePhone.IsEmpty())
		{
			itemNew.strMobilePhone = itemEx->strHomePhone;
		}
		if(itemNew.strMobilePhone.IsEmpty())
		{
			itemNew.strMobilePhone = itemEx->strOfficePhone;
		}

	}

	CString tempname;
	CString tempmobile;
	tempmobile = itemNew.strMobilePhone;
	tempname = itemNew.strName;
	tempname.TrimLeft();
	tempname.TrimRight();
	tempmobile.TrimLeft();
	tempmobile.TrimRight();
	
	if(tempmobile.IsEmpty()&&tempname.IsEmpty())
	{
		return false;
	}
	
	bool bOK = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.PhoneBookModify(m_Type, nID, tempmobile, tempname);
	if(bOK)
	{
		AddItem(itemNew, nID);
	}

	return bOK;
}

int CPhonebookEmbed::FindIndexByID(size_t nID)
{
	for(size_t i=0; i<Size(); ++i)
	{
		if(m_Data[i]->ID == nID)
			return i;
	}
	return -1;
}

bool	CPhonebookEmbed::Remove(size_t nID)
{
	Load();	
	int nIndex = FindIndexByID(nID);
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < Size());

	TPhoneBookItemEmbed* pItem = m_Data[nIndex];
	bool bOK = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.PhoneBookDelete(m_Type, pItem->ID);
	if(bOK)
	{
		delete pItem;
		m_Data.erase(m_Data.begin()+nIndex);
	}

	return bOK;
}

bool	CPhonebookEmbed::Modify(const TPhoneBookItem& item, size_t index)
{
	Load();
	int iID = FindIndexByID(index);
//	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager
	TPhoneBookItemEmbed* pItem = m_Data[iID];
	bool bOK = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.PhoneBookModify(m_Type, pItem->ID, item.strMobilePhone, item.strName);
	if(bOK)
	{
		pItem->strName = item.strName;
		pItem->strMobilePhone = item.strMobilePhone;
	}

	return bOK;
}


const	TPhoneBookItem& CPhonebookEmbed::GetAt(size_t index)const
{
	return * m_Data[index];
}

size_t	CPhonebookEmbed::Size()const
{
	return m_Data.size();
}


struct PBItemEmbedEqual
{
	const TPhoneBookItem& m_item;
	PBItemEmbedEqual(const TPhoneBookItem& item):m_item(item){}
	bool operator()(const TPhoneBookItemEmbed* item)const
	{
		return m_item.strName == item->strName && m_item.strMobilePhone == item->strMobilePhone;
	}
};

bool	CPhonebookEmbed::Find(const TPhoneBookItem& item, size_t* pIndex)
{
	std::vector<TPhoneBookItemEmbed*>::const_iterator iFind = 
		std::find_if(m_Data.begin(), m_Data.end(), PBItemEmbedEqual(item));

	if(iFind == m_Data.end())return false;

	if(pIndex)
	{
		*pIndex = iFind - m_Data.begin();
	}

	return true;
}

struct PBItemEmbed_NameOrderUp
{
	bool operator()(const TPhoneBookItemEmbed* itemA, const TPhoneBookItemEmbed* itemB)const
	{
		return itemA->strName < itemB->strName;
	}
};

struct PBItemEmbed_NameOrderDown
{
	bool operator()(const TPhoneBookItemEmbed* itemA, const TPhoneBookItemEmbed* itemB)const
	{
		return itemA->strName > itemB->strName;
	}
};

void	CPhonebookEmbed::SortByName(bool bUpOrder)
{
	if(bUpOrder)
		std::sort(m_Data.begin(), m_Data.end(), PBItemEmbed_NameOrderUp());
	else
		std::sort(m_Data.begin(), m_Data.end(), PBItemEmbed_NameOrderDown());
}

size_t  CPhonebookEmbed::MaxStorage()const
{
	if(m_MaxID>m_MinID)
	{
		return m_MaxID - m_MinID+1;
	}
	else
	{
		return 0;
	}
}
