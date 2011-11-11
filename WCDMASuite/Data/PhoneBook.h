#ifndef PHONEBOOK_H
#define PHONEBOOK_H


struct TPhoneBookItem
{
	CString strName;	//姓名
	CString strMobilePhone;//电话号码
};

struct TPhoneBookItemEx: public TPhoneBookItem
{
	int		nID;			//联系人唯一ID
	int		nParentID;		//父ID
	CString	strTypeName;	//类型名
	CString	strBusiness;	//职务
	CString strEmail;		//邮件
	CString strCountry;		//国家/地区
	CString strHomePhone;	//家庭电话
	CString strFax;			//传真
	CString strOffice;		//单位	
	CString strOfficePhone;	//办公电话
public:
	bool operator==(const TPhoneBookItemEx& item) const;
	BOOL operator=(const TPhoneBookItemEx& item);
	TPhoneBookItemEx();
};

class CPhonebook
{
protected:
	virtual ~CPhonebook(){}
public:
	virtual size_t	Size()const = 0;

	virtual void	SortByName(bool bUpOrder) = 0;
	virtual size_t  MaxStorage()const = 0;
};

#endif
