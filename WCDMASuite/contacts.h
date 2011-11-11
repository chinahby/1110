	//OutlookContact.h:   interface   for   the   COutlookContact   class.  
  //  
  //////////////////////////////////////////////////////////////////////  
#pragma once
 
#include "data\phonebookpc.h"
#include <vector>
#include <string>

using namespace std; 

typedef   struct __CONTACTS  
{  
	CString DisplayName; //char DisplayName[200];
	CString EmailAddr; //char EmailAddr[200];
	CString homePhone; // home phone number;
	CString mobilePhone; //mobile phone number;
	CString homeCountry;//home country
	CString homePostCode;//home post code
	CString homeProvince;//home province
	CString homeCity;//   home city
	CString homeStreetAddr;// home street address
	CString homeFax;//home fax
	CString company;//company
	CString officePhone; //office phone number;
	CString officeCountry;//company Country
	CString officePostCode;//company post code
	CString officeProvince;//company province
	CString officeCity;//company city
	CString officeStreetAddr;//company street address
    CString officeDuty;   //personal duty
	CString officeDepartment;//department
	CString officebureaux;//bureaux
	CString officeFax;//office fax
	CString officeCallnum;//Ñ°ºô»ú
	CString officeIPPhone;// office IP phone
  
}CONTACTS, *LPCONTACTS; 
 
class   COutlookContact      
{ 
public:  
	COutlookContact();  
	~COutlookContact();	
	bool   Open();  
	void   Close();  
	long   GetItemsCount();  
	bool   Insert2OutllookAllItems(const CPhonebookPC& itemList); 
	bool   ReadAllItemsFromOutlook();  
	
	
	vector<CONTACTS> outlookItemList;
private:  
	bool   m_bOpened;  
}; 

    