//   OutlookContact.cpp:   implementation   of   the   COutlookContact   class.  
//  
//////////////////////////////////////////////////////////////////////  

#include   "stdafx.h"  
#include   "contacts.h"
#include "ProgressWnd.h" 
#include "dlgmsg.h"
#include "GlobalDataCenter.h"
#include "ImportRepeatPrompt.h"
#include "Util.h"



//import   outlook   type   library   from   local   path  
#import "c:\Program Files\Common Files\Microsoft Shared\Office11\mso.dll" named_guids
#import "c:\Program Files\Microsoft Office\OFFICE11\MSOUTL.OLB" \
	no_namespace exclude("_IRecipientControl", "_DRecipientControl")  

 

#ifdef   _DEBUG  
#undef   THIS_FILE  
static   char   THIS_FILE[]=__FILE__;  
#define   new   DEBUG_NEW  
#endif  


_ApplicationPtr   m_pOutlookApp;  
_ItemsPtr pItems;
MAPIFolderPtr pFolder;
_ContactItemPtr pContact;

//////////////////////////////////////////////////////////////////////  
//   Construction/Destruction  
//////////////////////////////////////////////////////////////////////  

COutlookContact::COutlookContact()  
{  
	m_bOpened = false;
}  

COutlookContact::~COutlookContact()  
{  
  if(m_bOpened)
  {
	Close();
  }  
}  

bool   COutlookContact::Open()  
{  
	if(m_bOpened)
	{
		return   true; 
	}  

	CoInitialize(NULL); 
	CLSID clsidWord;
	::CLSIDFromProgID(L"Outlook.Application", &clsidWord); 
	try
	{	
		HRESULT   hr=m_pOutlookApp.CreateInstance(__uuidof(Application));  
		if(FAILED(hr))
		{
			return   false; 
		} 
	}
	catch(_com_error &e)
	{
		LPCTSTR   strMsg = e.ErrorMessage();  
		//GMessageBox(strMsg); 
		if(m_pOutlookApp)
		{
		m_pOutlookApp.Release();
		}
		
		return   false;	
	}

	
	//Now   m_pOutlookApp   can   work; 
	try
	{
		pFolder=m_pOutlookApp->GetNamespace(_bstr_t("MAPI"))->GetDefaultFolder(olFolderContacts);
	}
	catch(_com_error &e)
	{
		LPCTSTR   strMsg = e.ErrorMessage();  
		//GMessageBox(strMsg); 
		if(pFolder)
		{
		pFolder.Release();
		}
		if(m_pOutlookApp)
		{
		m_pOutlookApp.Release();
		}
		
		return   false;	
	}
	
	if (pFolder==NULL)
	{
		//GMessageBox("Could not find default contacts folder.");
		return false;
	}

	m_bOpened   =   true;	  
	return   true;  

}  

void   COutlookContact::Close()  
{  
  if(m_bOpened)  
  {  
	  if(pContact)
	  {
		pContact.Release();
	  }
	  if(pItems)
	  {
		pItems.Release();
	  }
	  if(pFolder)
	  {
	  pFolder.Release();  
	  }
	  if(m_pOutlookApp)
	  {
	  m_pOutlookApp.Release();  
	  }
	   

	  CoUninitialize(); 	   
	  m_bOpened   =   false;
	  return;
	}  
}  

long   COutlookContact::GetItemsCount()  
{  
  _ItemsPtr   pItems=pFolder->GetItems();  
  return   pItems->GetCount();  
}  

bool   COutlookContact::ReadAllItemsFromOutlook()  
{  
	CPhonebookPCClass pbClass;
	int nFailed = 0;
	int nsuccessed = 0;	
	int nCountItems = 0;
	int nCount = 0;
	int nCanceled = 0;
	int nOverlapped = 0;
	BOOL b_selApp2All = FALSE;//重复时是否应用到所有
	BOOL b_selOK = FALSE;//是否选择的是覆盖
	

	try
	{
		pItems=pFolder->GetItems();
	}
	catch(_com_error &e)
	{
		LPCTSTR   strMsg = e.ErrorMessage();  
		if(pItems)
		{
		pItems.Release();
		}
		//AfxMessageBox(strMsg);
		GMessageBox(strMsg);  
		return   false;	
	}
	
	if (pItems==NULL)
	{
//		AfxMessageBox("Unabel to get Contact Items.");
		GMessageBox(LoadStringEx(IDS_GETCONTACTITEM_FAIL));
		return true;
	}

	try
	{	
		nCountItems = pItems->GetCount();
		pContact = pItems->GetFirst();
	}
	catch(_com_error &e)
	{
		LPCTSTR   strMsg = e.ErrorMessage();
		if(pContact)
		{
		pContact.Release();
		}
		
		if(pItems)
		{
		pItems.Release();
		}
		GMessageBox(strMsg);  
		return   false;	
	}

	nCount = nCountItems;
	try
	{
		for(int i = 0; i < nCountItems; i++)
		{
			if (NULL == pContact)
			{
				nCount -= 1;
				pContact=pItems->GetNext();
				continue;
			}
			pContact=pItems->GetNext();
		}
	}
	catch(_com_error &e)
	{
		LPCTSTR   strMsg = e.ErrorMessage();
		if(pContact)
		{
		pContact.Release();
		}

		if(pItems)
		{
		pItems.Release();
		}
		
		GMessageBox(strMsg);  
		return   false;	
	}

	//进度显示
	CProgressWnd wndProgress(AfxGetMainWnd(), LoadStringEx(IDS_SAMESTEP));
	
	wndProgress.GoModal();
	wndProgress.SetRange(0,nCountItems);
	wndProgress.SetText(LoadStringEx(IDS_SAMESTEP_TIMES));
		
	
	try
	{
		pContact = pItems->GetFirst();
	}
	catch(_com_error &e)
	{
		LPCTSTR   strMsg = e.ErrorMessage();
		if(pContact)
		{
		pContact.Release();
		}
		if(pItems)
		{
		pItems.Release();
		}
		
		GMessageBox(strMsg);  
		return   false;	
	}

	//while(1)
	for(int j = 0; j < nCountItems; j++)
	{
		
		try
		{
			if (NULL == pContact)
			{
				pContact=pItems->GetNext();
				continue;
			}
		}
		catch(_com_error &e)
		{
			LPCTSTR   strMsg = e.ErrorMessage();
			if(pContact)
			{
			pContact.Release();
			}

			if(pItems)
			{
			pItems.Release();
			}
			
			GMessageBox(strMsg);  
			return   false;	
		}
		wndProgress.StepIt();
		wndProgress.PeekAndPump();
		
		if (wndProgress.Cancelled()) 
		{
			nFailed = nCount -nsuccessed - nCanceled-nOverlapped;
			GMessageBox(LoadStringEx(IDS_SYN_HASCANCEL));
			break;
		}
		CONTACTS info;
		CString strContactItem;
		TPhoneBookItemEx tempItem;
		try
		{
			tempItem.strBusiness = (char*)pContact->GetJobTitle();
			tempItem.strCountry = (char*)pContact->GetBusinessAddressCountry();
			tempItem.strEmail = (char*)pContact->GetEmail1Address();
			tempItem.strFax = (char*)pContact->GetBusinessFaxNumber();
			tempItem.strHomePhone = (char*)pContact->GetHomeTelephoneNumber();
			tempItem.strMobilePhone = (char*)pContact->GetMobileTelephoneNumber();
			tempItem.strName = (char*)pContact->GetFullName();
			tempItem.strOffice = (char*)pContact->GetCompanyName();
			tempItem.strOfficePhone = (char*)pContact->GetBusinessTelephoneNumber();
			tempItem.nParentID = 6;
		}
		catch(_com_error &e)
		{
			if(pContact)
			{
			pContact.Release();
			}

			if(pItems)
			{
			pItems.Release();
			}
			
			
			LPCTSTR   strMsg = e.ErrorMessage();  
			GMessageBox(strMsg); 
			return   false;	
		}

		CPhonebookPC pbPCContacts;
		if(pbPCContacts.IsExitRecord(tempItem, TRUE))
		{
			if(!b_selApp2All)
			{
				CString strRepeatInfo;
				CString strA(LoadStringEx(IDS_OUTLOOK_REPEAT_A));
				CString strB;
				CString strC(LoadStringEx(IDS_OUTLOOK_REPEAT_B));
				strB.Format(TEXT("%s"),tempItem.strName);
				strRepeatInfo = strA + strB;
				strRepeatInfo += strC;
				CImportRepeatPrompt repeatDlg(&wndProgress,strRepeatInfo);				
				
				if(IDOK == repeatDlg.DoModal())
				{
					b_selOK = TRUE;
					if(repeatDlg.m_app2All)
					{
						b_selApp2All = TRUE;
					}
					
					//nOverlapped +=1;
					bool bAddok1 = pbPCContacts.Add(tempItem);
					if(bAddok1)
					{
						nOverlapped +=1;
					}
					else
					{
						nFailed +=1;
					}
				}
				else
				{
					if(repeatDlg.m_app2All)
					{
						b_selApp2All = TRUE;
					}
					b_selOK = FALSE;
					nCanceled += 1;
				}				
			}
			else
			{
				if(b_selOK)
				{
					//nOverlapped +=1;
					bool bAddok = pbPCContacts.Add(tempItem);
					if(bAddok)
					{
						nOverlapped +=1;
					}
					else
					{
						nFailed +=1;
					}
				}
				else
				{
					nCanceled += 1;
				}
			}
		}
		else
		{
			bool bok = pbPCContacts.Add(tempItem);
			if(bok)
			{
				nsuccessed +=1;
			}
			else
			{
				nFailed +=1;
			}
		}
		
			
		try
		{
			pContact=pItems->GetNext();
		}
		catch(_com_error &e)
		{
			if(pContact)
			{
			pContact.Release();
			}

			if(pItems)
			{
			pItems.Release();
			}
			
			
			LPCTSTR   strMsg = e.ErrorMessage();  
			GMessageBox(strMsg); 
			return   false;	
		} 
	}

	wndProgress.m_bCancelled = true;
	wndProgress.Hide();
	if(nsuccessed + nOverlapped + nCanceled> 0 )//
	{
		
		if((nsuccessed+nFailed +nCanceled+nOverlapped) < nCount)
		{
			nFailed = nCount - nsuccessed - nCanceled-nOverlapped;
		}
		//完成后隐藏进度显示
		CString strMsg ;
		strMsg = LoadStringEx(IDS_SAMESTEP_A);
		CString strCount;
		strCount.Format(TEXT("%d"), nCount);
		strMsg += strCount + LoadStringEx(IDS_SAMESTEP_B);
		CString strSucc;
		strSucc.Format(TEXT("%d"), nsuccessed);
		strMsg += strSucc + LoadStringEx(IDS_SAMESTEP_C);
		CString strOverlapped;
		strOverlapped.Format(TEXT("%d"), nOverlapped);
		strMsg += strOverlapped + LoadStringEx(IDS_SAMESTEP_D);
		CString strCanncel;
		strCanncel.Format(TEXT("%d"), nCanceled);
		strMsg += strCanncel + LoadStringEx(IDS_SAMESTEP_E);
		CString strFail;
		strFail.Format(TEXT("%d"), nFailed);
		strMsg += strFail + LoadStringEx(IDS_OUTLOOK_INFO_F);		
		
//		strMsg.Format("尝试同步联系人条数共计： %d 条\r\n同步不重复记录的条数：%d条\r\n出现重复用户选择覆盖的条数：%d条\r\n出现重复被用户取消的条数：%d 条\r\n同步失败的条数：%d条",
//			nCount,nsuccessed,nOverlapped,nCanceled,nFailed);
		
		GMessageBox(strMsg) ;
	}
	else if ( (nsuccessed + nOverlapped) == 0 )
	{
		GMessageBox(LoadStringEx(IDS_CONTACTS_ISNULL));
	}
	else 
	{
		GMessageBox(LoadStringEx(IDS_SYN_FAIL));
	}


	return true;
}

bool   COutlookContact::Insert2OutllookAllItems(const CPhonebookPC& itemList)
{

	  
	_ContactItemPtr   spContact;  
	int   i;
	int succeeded =0;
	int failed = 0;
	int count = itemList.Size();
	//进度显示
	CProgressWnd wndProgress(AfxGetMainWnd(), LoadStringEx(IDS_SAMESTEP));
	
	wndProgress.GoModal();
	wndProgress.SetRange(0,count);
	wndProgress.SetText(LoadStringEx(IDS_SYNTO_OO_PROGRESS));
	
	try
	{
		for(i=0;i<count;i++)  
		{
		
			wndProgress.StepIt();
			wndProgress.PeekAndPump();
			
			if (wndProgress.Cancelled()) 
			{
				failed = count -succeeded;
	//			AfxMessageBox("同步操作已取消！",MB_OK);
				GMessageBox(LoadStringEx(IDS_SYN_HASCANCEL));
				break;
			}
			
			TPhoneBookItemEx item = itemList.GetAt(i);
			
			spContact = m_pOutlookApp->GetApplication()->CreateItem(olContactItem);  
			_bstr_t   strcompanyDuty = (LPCTSTR )item.strBusiness;	//职务
			_bstr_t   strEmail =   (LPCTSTR )item.strEmail;//邮件
			_bstr_t	  strCountry= (LPCTSTR )item.strCountry;  //国家/地区
			_bstr_t   strHomePhone = (LPCTSTR )item.strHomePhone;	//家庭电话
			_bstr_t   strFax = (LPCTSTR )item.strFax;			//传真
			_bstr_t   strOffice = (LPCTSTR )item.strOffice;		//单位
			_bstr_t   strOfficePhone= (LPCTSTR )item.strOfficePhone;	//办公电话
			_bstr_t   strMobilePhone = (LPCTSTR )item.strMobilePhone;//移动电话
			_bstr_t   strName = (LPCTSTR )item.strName;//姓名

  
			spContact->PutHomeAddressCountry(strCountry);
			spContact->PutFullName(strName);
			spContact->PutEmail1Address(strEmail);
			spContact->PutJobTitle(strcompanyDuty);
			spContact->PutHomeTelephoneNumber(strHomePhone);
			spContact->PutBusinessFaxNumber(strFax);
			spContact->PutCompanyName(strOffice);
			spContact->PutBusinessTelephoneNumber(strOfficePhone);
			spContact->PutMobileTelephoneNumber(strMobilePhone);
			spContact->PutFullName(strName);
			BOOL   bFlag=spContact->Save();  
			if(!bFlag)
			{
				succeeded +=1;
			}
			else
			{
				failed +=1;
			}
		}

		wndProgress.m_bCancelled = true;
		wndProgress.Hide();
		if(succeeded>0 )
		{
			
			if((succeeded+failed)<count)
			{
				failed = count - succeeded;
			}
			//完成后隐藏进度显示
			CString strMsg ;
			CString strA(LoadStringEx(IDS_TRY_SAMESTEP_A));
			CString strB;
			strB.Format(TEXT("%d"), count);
			strA = strA + strB;
			strA += LoadStringEx(IDS_TRY_SAMESTEP_B);
			CString strC;
			strC.Format(TEXT("%d"), succeeded);
			strA += strC;
			strA += LoadStringEx(IDS_TRY_SAMESTEP_C);
			CString strD;
			strD.Format(TEXT("%d"), failed);
			strA += strD;
			strA += LoadStringEx(IDS_OUTLOOK_INFO_F);
			strMsg = strA;
		//	strMsg.Format("尝试同步 %d 条联系人记录，成功%d条,失败%d条！",count,succeeded,failed);
			
			GMessageBox(strMsg,TEXT(""),MB_OK|MB_ICONASTERISK) ;
		}
		else if ( succeeded == count )
		{
	//		AfxMessageBox("同步的联系人记录为空",MB_OK) ;
			GMessageBox(LoadStringEx(IDS_CONTACTS_ISNULL));
		}
		else 
		{
	//		AfxMessageBox("同步失败");
			GMessageBox(LoadStringEx(IDS_SYN_FAIL));
		}
	}catch(_com_error &e)
	{
		if(spContact)
		{
			spContact.Release();
		}

		if(m_pOutlookApp)
		{
			m_pOutlookApp.Release();
		}
		LPCTSTR   strMsg = e.ErrorMessage();  
		GMessageBox(strMsg); 
		return   true;	
	}
	
	return true;
}  




