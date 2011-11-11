#ifndef AFX_PHONEBOOKEMBED_H__F85817C7_4F7F_46F4_9A27_8ED2665AC29F__INCLUDED_
#define AFX_PHONEBOOKEMBED_H__F85817C7_4F7F_46F4_9A27_8ED2665AC29F__INCLUDED_

#include "PhoneBook.h"
#include <vector>

struct TPhoneBookItemEmbed: public TPhoneBookItem
{
	int ID;
};


class CMobileManager;
class CPhonebookEmbed : public CPhonebook  
{
public:
	CPhonebookEmbed(const CString&  type);
	virtual ~CPhonebookEmbed();

	void			SetIDRange(int max){m_MaxID = max;}
	int			GetIDRange()const{return m_MaxID;}
	void			Load();
	const CString&	Type()const{return m_Type;}
	bool IsFull(){return m_bFull;}
public:
	virtual bool	Add(const TPhoneBookItem& item);
	virtual bool	Remove(size_t index);
	virtual bool	Modify(const TPhoneBookItem& item, size_t index);
	virtual const	TPhoneBookItem& GetAt(size_t index)const; 
	virtual size_t	Size()const;

	virtual bool	Find(const TPhoneBookItem& item, size_t* pIndex);
	virtual void	SortByName(bool bUpOrder);
	virtual size_t  MaxStorage()const;
	int FindIndexByID(size_t nID);
	void SetReadResult(bool bRead){m_bLoaded = bRead;TRACE(TEXT("SetReadResult m_bLoaded = %d\n"), m_bLoaded);}
	bool IsLoadSim(){return m_bLoaded;}
	void Clean();
private:
	
	void AddItem(const TPhoneBookItem& item, int id);
	int  FreePlace()const;

	friend class CThreadLoadPB;
private:
	CPhonebookEmbed(const CPhonebookEmbed&);
	CPhonebookEmbed& operator=(const CPhonebookEmbed&);
private:
	//CMobileManager& m_Mgr;
	const CString   m_Type;

	int m_MinID;
	int m_MaxID;
	bool m_bLoaded;

	std::vector<TPhoneBookItemEmbed*> m_Data;

	bool m_bFull;
};

#endif
