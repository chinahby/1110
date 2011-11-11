#include "stdafx.h"
#include "ThreadLoadPB.h"
#include "MobileManager.h"
#include "ATResponse.h"
#include "Data\PhonebookEmbed.h"
#include "SunnyCatDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CThreadLoadPB::CThreadLoadPB(CMobileManager& mgr,CPhonebookEmbed& PB)
:m_Mgr(mgr),m_PB(PB)
{

}

CThreadLoadPB::~CThreadLoadPB()
{

}

void CThreadLoadPB::OnPBRead(const TPhoneBookItem& item, int id)
{
	if((!item.strMobilePhone.IsEmpty())||(!item.strName.IsEmpty()))
	{
		m_PB.AddItem(item, id);
	}
	
	CString str;
	str.Format(TEXT("%s [%s]"), item.strName, item.strMobilePhone);
	int len = m_PB.Size();
	int maxLen = m_PB.MaxStorage();

	m_pUI->SetProgress(len,maxLen, str);
}

void CThreadLoadPB::SetIDRange(int maxID)
{
	m_PB.SetIDRange(maxID);
}

void CThreadLoadPB::DoWork()
{
	//选择存储器
	
	CATCmd cmd = m_Mgr.SendATPacket(CMobileATProto::PhoneBookSelect(m_PB.Type()));

	if(RETURN_OK != m_Mgr.WaitForATCompleted(cmd,2000))
	{
		return;
	}


	CResponseCPBW* parserQueryParam = static_cast<CResponseCPBW*>(m_Mgr.FindParser("+CPBW"));
	if(parserQueryParam) parserQueryParam->SetLoadingThread(this);

	//查询范围
	bool bOK =false; 
	for(size_t i=0;i<10;i++)
	{
		cmd = m_Mgr.SendATPacket(CMobileATProto::PhoneBookType());
		int nReturn = m_Mgr.WaitForATCompleted(cmd,2000);
		if(nReturn == RETURN_ERROR_REPORT+14)
		{
			bOK = false;
			Sleep(2000);
			continue;
		}
		else if(nReturn == RETURN_OK)
		{
			bOK = true;
			break;
		}
	}

	if(bOK)
	{
		CResponseCPBR* parserRead = static_cast<CResponseCPBR*>(m_Mgr.FindParser("+CPBR"));
		if(parserRead) parserRead->SetLoadingThread(this);
		//读取电话本
		
		int min = 1;
		int maxItem = 1;
		maxItem = m_PB.GetIDRange();

		cmd = m_Mgr.SendATPacket(CMobileATProto::PhoneBookLoad(min, maxItem));
		m_Mgr.WaitForATCompleted(cmd,60000);//读取电话本的时间可能比较长

		if(parserRead) parserRead->SetLoadingThread(NULL);
	}

	if(parserQueryParam) parserQueryParam->SetLoadingThread(NULL);
	m_Mgr.PostMessageToClient(WM_SIMREAD_RESULT,bOK);
}
