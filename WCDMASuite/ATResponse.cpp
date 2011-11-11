#include "stdafx.h"

#include "ATResponse.h"
#include "MobileManager.h"
#include "Util.h"
#include "ComParser.h"
#include "CallManager.h"
#include "Porting.h"
#include "CDMATime.h"
#include "GlobalDataCenter.h"
#include "ThreadLoadPB.h"
#include "data/Phonebook.h"

#include "PDU.h"
#include "SunnyCatDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

enum
{
	RXRECEIVE_ASCII_COMMAND = 0,
	RXRECEIVE_ONE_SMS_UNICODE_LINE,
	RXRECEIVE_ONE_SMS_ASCII_LINE,
	RXRECEIVE_ONE_PHONEBOOK_LINE,
};


char CATResponse::s_RxBuff[RXBUFF_SIZE] = {0};
int  CATResponse::s_RxBuffSize = 0;

void CATResponse::ResetRxBuf()
{
	memset(s_RxBuff,0, sizeof(s_RxBuff));
	s_RxBuffSize = 0;
}

CATResponse::CATResponse(const CString& strName, CMobileManager* pMgr)
:m_strName(strName),
 m_pMgr(pMgr)
{

}

CATResponse::~CATResponse()
{

}

bool CATResponse::MatchByName(const CString& name)const
{
	return name.CompareNoCase(m_strName)==0;
}

bool CATResponse::RecvLine(char cRecv)
{
	if(cRecv =='\r' || cRecv=='\n')
	{
		s_RxBuff[s_RxBuffSize] = 0;
		return true;
	}

	s_RxBuff[s_RxBuffSize] = cRecv;
	++s_RxBuffSize;
	if(s_RxBuffSize>=RXBUFF_SIZE)
	{
		ResetRxBuf();
	}

	return false;
}


CSimpleResponse::CSimpleResponse(const CString& strName, CMobileManager* pMgr)
:CATResponse(strName, pMgr)
{
}

bool CSimpleResponse::OnRecvChar(char cRecv)
{
	bool bComplete = RecvLine(cRecv);

	if(bComplete)
	{
		HandleAT();
	}

	return  bComplete;
}

//**************************************************
//	基本的AT处理
//**************************************************

CNormalResponse::CNormalResponse()
:CATResponse("", NULL)
{
}

static CString AssignString(const char* str, size_t size)
{
	char buf[512];
	size = min(size,sizeof(buf)-1);
	memcpy(buf,str,size);
	buf[size]=0;

	return buf;
}

CString CNormalResponse::GetResponseString()const
{
	char* str = s_RxBuff;
	char* end = str+s_RxBuffSize;
	for(;str<end && *str ==' ';++str);
	for(;str<end && *str!='+';++str);
	if(str==end)str=s_RxBuff;

	char* c=str;
	for(;c<end && *c!=':'; ++c);
	if(c==end)return str;
	for(--c ;c>str && *c==' ';--c);

	return AssignString(str,c-str+1);
}

bool CNormalResponse::OnRecvChar(char cRecv)
{
	if(cRecv =='\r' || cRecv=='\n' || cRecv==':')
	{
		if(cRecv == ':')
		{
			s_RxBuff[s_RxBuffSize] = cRecv;
			++s_RxBuffSize;
		}
		return s_RxBuffSize>0;
	}
	else
	{
		if(!cRecv)return false;

		s_RxBuff[s_RxBuffSize] = cRecv;
		++s_RxBuffSize;
		if(s_RxBuffSize>=RXBUFF_SIZE)
		{
			ResetRxBuf();
		}
		return false;
	}
}

/*
//**************************************************
//+CDS 短信状态报告
//**************************************************
CResponseCDS::CResponseCDS(CMobileManager* pMgr)
:CSimpleResponse("+CDS", pMgr)
{

}

void CResponseCDS::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	//m_pMgr->m_StatusReport = atoi(parser.GetParsedParam(7));

	m_pMgr->m_EventStatusReport.SetEvent();
}
*/


//**************************************************
//+CSQ 查询信号强度
//**************************************************
CResponseCSQ::CResponseCSQ(CMobileManager* pMgr)
:CSimpleResponse("+CSQ", pMgr)
{
}

void CResponseCSQ::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
	{
		int iPara = atoi(parser.GetParsedParam(1).c_str());
		m_pMgr->m_nSignal = iPara;
		///通知界面
		m_pMgr->PostMessageToClient(WM_SHOW_SIGNAL, iPara, 0);
	}
}

//**************************************************
//^HDRCSQ 查询信号强度
//**************************************************
CResponseHDRCSQ::CResponseHDRCSQ(CMobileManager* pMgr)
:CSimpleResponse("^HDRCSQ", pMgr)
{
}

void CResponseHDRCSQ::HandleAT()
{
/*************************************************************/
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
	{
		int iPara = atoi(parser.GetParsedParam(1).c_str());
		m_pMgr->m_nHDRSignal = iPara;
		///通知界面
		TRACE(_T("WM_SHOW_HDRSIGNAL"));
		m_pMgr->PostMessageToClient(WM_SHOW_HDRSIGNAL, iPara, 0);
	}
/*************************************************************/
}

//**************************************************
//^PREFMODE
//**************************************************
CResponsePREFMODE::CResponsePREFMODE(CMobileManager* pMgr)
:CSimpleResponse("^PREFMODE", pMgr)
{
}

void CResponsePREFMODE::HandleAT()
{
/*************************************************************/
	CComParser parser;
	parser.ParseString(s_RxBuff);

	int iPara = atoi(parser.GetParsedParam(1).c_str());
//	m_pMgr->m_iNetMode = iPara;
	///通知界面
	TRACE(TEXT("CResponsePREFMODE MODE = %d\n"), iPara);
//	m_pMgr->PostMessageToClient(WM_SHOW_HDRSIGNAL, iPara, 0);

/*************************************************************/
}

//**************************************************
//^MODE
//**************************************************
CResponseMODE:: CResponseMODE(CMobileManager * pMgr)
:CSimpleResponse("^MODE", pMgr)
{
}

void CResponseMODE::HandleAT()
{
//*************************************************************
	CComParser parser;
	parser.ParseString(s_RxBuff);

	int iPara = atoi(parser.GetParsedParam(1).c_str());
	if (m_pMgr->m_iNetMode == iPara)
		m_pMgr->m_bSetNetModeOK = true;

	///通知界面
	TRACE(_T("CResponseMODE :> MODE = %d\n"), iPara);
//	m_pMgr->PostMessageToClient(WM_SHOW_HDRSIGNAL, iPara, 0);

/*************************************************************/
}

//**************************************************
//+CREG 查询网络注册状态
//**************************************************
CResponseCREG::CResponseCREG(CMobileManager* pMgr)
:CSimpleResponse("+CREG", pMgr)
{
}

void CResponseCREG::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	int count = parser.GetParsedParamCount();
	if((2 == count)||(4 == count))
	{
		int iPara = atoi(parser.GetParsedParam(1).c_str());
		m_pMgr->m_registerStatus = iPara;
		///通知界面
		m_pMgr->PostMessageToClient(WM_REG_STATUS, iPara, 0);
	}

	

	if((3 == count)||(5 == count))
	{
		int iPara = atoi(parser.GetParsedParam(2).c_str());
		m_pMgr->m_registerStatus = iPara;
		///通知界面
		m_pMgr->PostMessageToClient(WM_REG_STATUS, iPara, 0);
	}
	
}

//**************************************************
//+GPRS  查询GPRS附着
//**************************************************
CResponseCGATT::CResponseCGATT(CMobileManager* pMgr)
:CSimpleResponse("+CGATT", pMgr)
{
}

void CResponseCGATT::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
	{
		int iPara = atoi(parser.GetParsedParam(1).c_str());
		if(iPara)
		{
			m_pMgr->m_netGprs = 1;
			m_pMgr->PostMessageToClient(WM_NET_TYPE, iPara, 0);
		}
		else
		{
			m_pMgr->m_netGprs = 0;
			m_pMgr->PostMessageToClient(WM_NET_TYPE, iPara, 0);
		}
		///通知界面
		//m_pMgr->PostMessageToClient(WM_NET_TYPE, iPara, 0);
	}
}


//**************************************************
//*EDGE  查询EDGE附着
//**************************************************
CResponseEDGE::CResponseEDGE(CMobileManager* pMgr)
:CSimpleResponse("*EDGE", pMgr)
{
}

void CResponseEDGE::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
	{
		int iPara = atoi(parser.GetParsedParam(1).c_str());
		if(iPara)
		{
			m_pMgr->m_netEdge = 1;
			iPara = 2;
			m_pMgr->PostMessageToClient(WM_NET_TYPE, iPara, 0);
		}
		else
		{
			m_pMgr->m_netEdge = 0;
			m_pMgr->PostMessageToClient(WM_NET_TYPE, iPara, 0);
		}
		///通知界面
		//m_pMgr->PostMessageToClient(WM_NET_TYPE, iPara, 0);
	}
}

//**************************************************
//*IMEI  查询IMEI
//**************************************************
CResponseIMEI::CResponseIMEI(CMobileManager* pMgr)
:CSimpleResponse("*WID", pMgr)
{
}

void CResponseIMEI::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
	{
		std::string str = parser.GetParsedParam(1);
		m_pMgr->m_imei = ToUnicode(str).c_str();
	
	}
}
//**************************************************
//*SOFTWV  sim card lock test
//**************************************************
CResponseSimLock::CResponseSimLock(CMobileManager* pMgr)
:CSimpleResponse("+CLCK", pMgr)   	// "*SOFTWV"
{
}

void CResponseSimLock::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
//	if(parser.GetParsedParamCount()>=2)
	{
		int iPara = atoi(parser.GetParsedParam(1).c_str());
		if(1 == iPara)
		{
			TRACE(TEXT("CResponseSimLock::HandleAT "));
			m_pMgr->m_simLocked = true;
			m_pMgr->PostMessageToClient(WM_SIGNAL_SIMLOCK, 0, 0);
		}
		else
		{
			m_pMgr->m_simLocked = false;
		}
	}
}

//**************************************************
//^CONN 呼叫接续
//**************************************************
CResponseCONN::CResponseCONN(CMobileManager* pMgr)
:CSimpleResponse("^CONN", pMgr)
{
}

void CResponseCONN::HandleAT()			// modify by lab686 20091029
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	
	int iPara = atoi(parser.GetParsedParam(1).c_str());
	m_pMgr->m_callnum = iPara;
	
	m_pMgr->GetCallManager().OnCallContinue();
}

//**************************************************
//^ORIG 正在发起呼叫
//**************************************************
CResponseORIG::CResponseORIG(CMobileManager* pMgr)
:CSimpleResponse("^ORIG", pMgr)
{
}

void CResponseORIG::HandleAT()
{
	m_pMgr->GetCallManager().OnOutgoingCall();
}

//**************************************************
//+RING 来电震铃
//**************************************************
CResponseRING::CResponseRING(CMobileManager* pMgr)
:CSimpleResponse("RING", pMgr)
{
}


void CResponseRING::HandleAT()
{
	m_pMgr->GetCallManager().OnIncomingCall("");
}


//**************************************************
//+CLIP 来电显示
//**************************************************
CResponseCLIP::CResponseCLIP(CMobileManager* pMgr)
:CSimpleResponse("+CLIP", pMgr)
{
}

void CResponseCLIP::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);

	m_pMgr->GetCallManager().OnIncomingCall(parser.GetParsedParam(1).c_str());
	m_pMgr->PostMessageToClient(WM_CALL_INCOMING, 0, 0);
}


//**************************************************
// ^CEND 电话结束
//**************************************************
CResponseCEND::CResponseCEND(CMobileManager* pMgr)
:CSimpleResponse("^CEND", pMgr)
{
}

void CResponseCEND::HandleAT()
{
	TRACE(TEXT("CResponseCEND::HandleAT:> OnCallEnd"));
	CComParser parser;
	parser.ParseString(s_RxBuff);

	int nReason = 0;
	if(parser.GetParsedParamCount()>1)
		nReason = atoi(parser.GetParsedParam(1).c_str());
	m_pMgr->GetCallManager().OnCallEnd(nReason);			// nReadon is no use..
}

//**************************************************
//+CIND电话结束
//**************************************************
CResponseCIND::CResponseCIND(CMobileManager* pMgr)
:CSimpleResponse("+CIND", pMgr)
{
}

void CResponseCIND::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);

	int isCallEnd = 0;
	if(parser.GetParsedParamCount()>=6)
	{
		isCallEnd = atoi(parser.GetParsedParam(5).c_str());
	}
	
	if(isCallEnd)
	{
		//m_pMgr->GetCallManager().OnCallEnd(0);
		//m_pMgr->GetCallManager().AcceptIncomingCall();
		m_pMgr->GetCallManager().OnCallContinue();
		
	}
	else
	{
		if(m_pMgr->GetCallManager().m_setCallStatus == CALLSTATE_IN_TALKING)
		{
			m_pMgr->GetCallManager().OnCallEnd(0);
		}
	}
	
}

//**************************************************
//+CPAS电话结束
//**************************************************
CResponseCPAS::CResponseCPAS(CMobileManager* pMgr)
:CSimpleResponse("+CPAS", pMgr)
{
}

void CResponseCPAS::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);

	int nState = 0;
	if(parser.GetParsedParamCount()>=1)
	{
		nState = atoi(parser.GetParsedParam(1).c_str());
	}
	
	if(nState == 3 || nState == 4)
	{
		
		
	}
	else if(nState == 0)
	{
		if(m_pMgr->GetCallManager().m_setCallStatus == CALLSTATE_INCOMING)
		{
			m_pMgr->GetCallManager().OnCallEnd(0);
		}
	}
	
}

//**************************************************
//+CLVL 音量
//**************************************************
CResponseCLVL::CResponseCLVL(CMobileManager* pMgr)
:CSimpleResponse("+CLVL", pMgr)
{
}

void CResponseCLVL::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	
	m_pMgr->m_nVolumn = atoi(parser.GetParsedParam(1).c_str());
}

/*
//**************************************************
//+ZMGF 信箱已满
//**************************************************
CResponseZMGF::CResponseZMGF(CMobileManager* pMgr)
:CSimpleResponse("+ZMGF", pMgr)
{
}

void CResponseZMGF::HandleAT()
{
	m_pMgr->PostMessageToClient(WM_SMS_FULL, 0, 0);
}
*/

//**************************************************
//+CMTI 收到新短信
//**************************************************
CResponseCMTI::CResponseCMTI(CMobileManager* pMgr)
:CSimpleResponse("+CMTI", pMgr)
{
}

void CResponseCMTI::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	int iPara = atoi(parser.GetParsedParam(2).c_str());

	//判断短信是否已满
	if(m_pMgr->m_iSMSCount == m_pMgr->m_iSMSMaxNum)
	{
		m_pMgr->PostMessageToClient(WM_SMS_FULL, 0, 0);
	} 
	else
	{
		m_pMgr->m_bIsAboveSendSMS = true;
		if ( !m_pMgr->m_bIsSendSMS )
		{
			TRACE(TEXT("PostMessageToClient :> WM_SMS_NOTIFY\n"));
			m_pMgr->PostMessageToClient(WM_SMS_NOTIFY, iPara, 0);
		}
		else
		{
			m_pMgr->ProcessNewSMSPostMsg(iPara);
		}
	}
	
	
}

//**************************************************
// 新收到的短信状态报告+CDSI
//**************************************************
CResponseSMSCDSI::CResponseSMSCDSI(CMobileManager* pMgr)
:CSimpleResponse("+CDSI", pMgr)	
{
}
void CResponseSMSCDSI::HandleAT()
{	
}

//**************************************************
// 10. 新短信状态报告直接上报指示^HCDS
//**************************************************
CResponseSMSHCDS::CResponseSMSHCDS(CMobileManager* pMgr)
:CSimpleResponse("^HCDS", pMgr)	
{
}
void CResponseSMSHCDS::HandleAT()
{	
}

//**************************************************
//短信发送成功上报指示+CMGS:
//**************************************************
CResponseSMSCMGS::CResponseSMSCMGS(CMobileManager* pMgr)
:CSimpleResponse("+CMGS", pMgr)	
{
}
void CResponseSMSCMGS::HandleAT()
{
	TRACE(TEXT("RECEIVE THE  +CMGS"));
	m_pMgr->PostMessageToClient(WM_SMSSEND_STATE, 1, 0);
}

//**************************************************
//短信发送失败上报指示+CMS ERROR:
//**************************************************
CResponseSMSERROR::CResponseSMSERROR(CMobileManager* pMgr)
:CSimpleResponse("+CMS ERROR", pMgr)	
{
}
void CResponseSMSERROR::HandleAT()
{
	TRACE(TEXT("RECEIVE THE  HCMGSF"));
	m_pMgr->PostMessageToClient(WM_SMSSEND_STATE, 0, 0);
}

//**************************************************
//FULL 短信满提示
//**************************************************
CResponseSMSFULL::CResponseSMSFULL(CMobileManager* pMgr)
:CSimpleResponse("^SMMEMFULL", pMgr)
{
}

void CResponseSMSFULL::HandleAT()
{
	m_pMgr->PostMessageToClient(WM_SMS_FULL, 0, 0);	
}

//**************************************************
//SIM锁定提示
//**************************************************
CResponseSIMER::CResponseSIMER(CMobileManager* pMgr)
:CSimpleResponse("SIMER", pMgr)
{
}

void CResponseSIMER::HandleAT()
{
	m_pMgr->m_simLocked = true;
}

//**************************************************
//+CMGR 读短信
//**************************************************
CResponseCMGR::CResponseCMGR(CMobileManager* pMgr)
:CATResponse("+CMGR", pMgr),
 m_PDULen(0)
{
	m_PDUBytes = NULL;
}

CResponseCMGR::~CResponseCMGR()
{
	delete[] m_PDUBytes;
	m_PDUBytes = NULL;
}

static WORD Str2Byte(const char* c)
{
	WORD num = 0;
	WORD i  = 0;
	while (c[i] >= '0' && c[i] <= '9')
	{
		num = num * 10 + ( c[i] - '0' );
		++i;
	}
	return num;
}

bool CResponseCMGR::OnRecvChar(char cRecv)
{
	if(!m_PDUBytes)
	{
		bool bComplete = RecvLine(cRecv);
		if(bComplete)
		{
			CComParser parser;
			parser.ParseString(s_RxBuff);
			if(parser.GetParsedParamCount()<2)
			{
				m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR);
				return true;
			}
			
			///短信状态
			const std::string& state = parser.GetParsedParam(1); 
			if(state == "0")		///收件箱、未读
			{
				m_pMgr->m_NewSMS.iSMSState = SMS_UNREAD;
			}
			else if(state == "1")		///收件箱、已读
			{
				m_pMgr->m_NewSMS.iSMSState = SMS_READ;
			}
			else if(state == "2")	///发件箱、未发送 
			{
				m_pMgr->m_NewSMS.iSMSState = SMS_UNSEND;
			}
			else									///发件箱、已发送
			{
				m_pMgr->m_NewSMS.iSMSState = SMS_SEND;
			}

			m_PDULen = 0;
			m_PDUBytes = new char[512];
			ZeroMemory(m_PDUBytes, 512);
			

			ResetRxBuf();
		}
		return false;
	}
	else
		return ReadContent(cRecv);
}

bool CResponseCMGR::ReadContent(char cRecv)
{
	if((cRecv == '\n')&& !m_PDULen)//信息行的结束
		return false;
	else if(cRecv == '\r')
	{
		ExtractPDU();
		SMSReadingOver();		
		return true;
	}
	else
	{
		m_PDUBytes[m_PDULen++] = cRecv;
		return false;
	}
}

void CResponseCMGR::Clean()
{
	delete[] m_PDUBytes;
	m_PDUBytes = NULL;
	m_PDULen = 0;
}

static BYTE Char2Byte(char c)
{
	if(c >='0' && c<='9')
		return c-'0';
	else if(c>='A' && c<='F')
		return c-'A'+10;
	else if(c>='a' && c<='f')
		return c-'a'+10;
	else
		ASSERT(0);

	return 0;
}

void CResponseCMGR::ExtractPDU()
{	
	ASSERT(!(m_PDULen%2));

	BYTE* pduBytes = new BYTE[m_PDULen/2+1];
	int nByteCount = 0;
	for(int i=0; i<m_PDULen; i+=2)
	{
		BYTE h = Char2Byte(m_PDUBytes[i]);
		BYTE l = Char2Byte(m_PDUBytes[i+1]);
		pduBytes[nByteCount] = (h&0X0F)<<4 | (l&0X0F);

		++nByteCount;
	}

	PDU::PDUIn pdu(pduBytes, nByteCount,m_pMgr->m_NewSMS.iSMSState);
	m_pMgr->m_NewSMS.strText = pdu.Text().c_str();
	m_pMgr->m_NewSMS.strPhoneNum = pdu.Number().c_str();

	if((m_pMgr->m_NewSMS.iSMSState==0)||(m_pMgr->m_NewSMS.iSMSState==1))
	{
		const PDU::Timestamp tm = pdu.Time();
		m_pMgr->m_NewSMS.tmSMSTime = CTime(tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
	}

	delete[] pduBytes;
}

void CResponseCMGR::SMSReadingOver()
{
	Clean();

	//TRACE(" SMS:%s \n",m_pMgr->m_NewSMS.strText);
	ResetRxBuf();

	m_pMgr->m_SerialPort.SetReturnCode(RETURN_OK);
}


//**************************************************
//+CPMS 选择短信存储器
//**************************************************
CResponseCPMS::CResponseCPMS(CMobileManager* pMgr)
:CSimpleResponse("+CPMS", pMgr)
{
}

void CResponseCPMS::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	
	///存储的短信量
	if(parser.GetParsedParamCount()<3)return;
	m_pMgr->m_iSMSMaxNum = atoi(parser.GetParsedParam(2).c_str());
	m_pMgr->m_iSMSCount  = atoi(parser.GetParsedParam(1).c_str());
}

//*****************************************************************
//+CPBW 读取电话本范围可存储的最大条数
//*****************************************************************
CResponseCPBW::CResponseCPBW(CMobileManager* pMgr)
:CSimpleResponse("+CPBW", pMgr),
 m_ThreadLoadPB(NULL)
{
}

void CResponseCPBW::SetLoadingThread(CThreadLoadPB* thread)
{
	m_ThreadLoadPB = thread;
}

void CResponseCPBW::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	
	if(parser.GetParsedParamCount()<2)return;
	//int maxItem = atoi(parser.GetParsedParam(1));
	CString strMaxItem;
	int maxItem = 1;
	std::string str = parser.GetParsedParam(1);
	strMaxItem = ToUnicode(str).c_str();
	strMaxItem.TrimLeft();
	strMaxItem.TrimRight();
	int pos = strMaxItem.Find('-');
	if(-1 != pos)
	{
		strMaxItem.Delete(0,pos+1);
	}
	 
	int pos2 = strMaxItem.Find(')');
	if(-1 != pos2)
	{
		strMaxItem.Delete(pos2,1);
	}

	maxItem = _ttoi(strMaxItem);
		
	if(m_ThreadLoadPB)
	{
		m_ThreadLoadPB->SetIDRange(maxItem);
	}
}

//**************************************************
//+CPBR 读取电话本
//**************************************************
CResponseCPBR::CResponseCPBR(CMobileManager* pMgr)
:CSimpleResponse("+CPBR", pMgr)
{
}

void CResponseCPBR::SetLoadingThread(CThreadLoadPB* thread)
{
	m_ThreadLoadPB = thread;
}

void CResponseCPBR::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);

	int count = parser.GetParsedParamCount();

	if(count < 5)
	{
		return;
	}
	
	TPhoneBookItem item;

	CString mobile;
	CString username;
	std::string str = parser.GetParsedParam(2);
	mobile = ToUnicode(str).c_str();
	mobile.TrimLeft();
	mobile.TrimRight();

	str = parser.GetParsedParam(4);
	username = ToUnicode(str).c_str();
	username.TrimLeft();
	username.TrimRight();


	if((mobile.IsEmpty())||(TEXT(" ") ==mobile))
	{
		item.strMobilePhone = "";
	}
	else
	{
		item.strMobilePhone = mobile;
	}

	if((username.IsEmpty())||(TEXT(" ") ==username))
	{
		item.strName = TEXT("");
	}
	else
	{
		item.strName = ExtractUnicodeName(username);
	}

	int iPhoneBookId = atoi(parser.GetParsedParam(1).c_str());

	if(m_ThreadLoadPB)
	{
		m_ThreadLoadPB->OnPBRead(item, iPhoneBookId);
	}

//	TRACE3("PHBK:%d, \"%s\", \"%s\"\n", iPhoneBookId, item.strName, item.strMobilePhone);
	
}


void UnicodeToASCII(LPCWSTR SourceStr, DWORD dwWCharCount, LPSTR pASCII, DWORD dwASCIISize);
//解析电话本
CString CResponseCPBR::ExtractUnicodeName(const CString& Unicode)
{
	//将字符串序列转换成字节序列
	vector<BYTE> bytes;
	bytes.reserve(Unicode.GetLength()/2);

	ASSERT(!(Unicode.GetLength()%4)); //必须是4的倍数

	for(int i=0; i<Unicode.GetLength(); i+=4)
	{
		BYTE h = Char2Byte( Unicode[i] );
		BYTE l = Char2Byte( Unicode[i+1] );

		BYTE h1 = Char2Byte( Unicode[i+2] );
		BYTE l1 = Char2Byte( Unicode[i+3] );

		BYTE b = (h<<4) | l;
		BYTE b1 = (h1<<4) | l1;
		bytes.push_back(b1);
		bytes.push_back(b);
	}

	//将字节转换成Unicode
	vector<char> str;
	str.resize(bytes.size()+1);//20070303
	UnicodeToASCII((LPCWSTR)&bytes[0], bytes.size()/2, &str[0], str.size());
	return &str[0];
}

/*
//**************************************************
//+CNUM 读取本机号码
//**************************************************
CResponseCNUM::CResponseCNUM(CMobileManager* pMgr)
:CSimpleResponse("+CNUM", pMgr)
{
}

void CResponseCNUM::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
		m_pMgr->m_strLocalNum = parser.GetParsedParam(1);
	else
		m_pMgr->m_strLocalNum = "";

	m_pMgr->m_Options.m_strLocalPhone = m_pMgr->m_strLocalNum;
}
*/


//**************************************************
//+CSCA 读取短信中心地址
//**************************************************
CResponseCSCA::CResponseCSCA(CMobileManager* pMgr)
:CSimpleResponse("+CSCA", pMgr)
{
}

void UnicodeToASCII(LPCWSTR SourceStr, DWORD dwWCharCount, LPSTR pASCII, DWORD dwASCIISize);

static CString UCS2Number(const CString& str)
{
	vector<BYTE> bytes; 
	bytes.reserve(256);
	for(int i=0; i<str.GetLength(); i+=4)
	{
		BYTE h = Char2Byte(str[i]);
		BYTE l = Char2Byte(str[i+1]);

		BYTE a = h<<4 | l;

		h = Char2Byte(str[i+2]);
		l = Char2Byte(str[i+3]);

		BYTE b = h<<4 | l;

		bytes.push_back(b);
		bytes.push_back(a);
	}

	char asc[256] = {0};
	UnicodeToASCII((WCHAR*)(&bytes[0]), bytes.size()/2, asc, sizeof(asc));

	return asc;
}

void CResponseCSCA::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
	{
		std::string str = parser.GetParsedParam(1);
		m_pMgr->m_strCscaNum = UCS2Number(ToUnicode(str).c_str());
	}
	else
		m_pMgr->m_strCscaNum = "";
}

//**************************************************
//S/W VER: 读取嵌入端软件版本
//**************************************************
CResponseATI3::CResponseATI3(CMobileManager* pMgr)
:CSimpleResponse("S/W VER", pMgr)
{

}

void CResponseATI3::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	std::string str = parser.GetParsedParam(1);
	m_pMgr->m_strLocalVersion = ToUnicode(str).c_str();
}

//**************************************************
//+ZDPB //选择默认电话本存储器，目前用来判断有卡无卡
//**************************************************
CResponseZDPB::CResponseZDPB(CMobileManager* pMgr)
:CSimpleResponse("+ZDPB", pMgr)
{
}

void CResponseZDPB::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	std::string str = parser.GetParsedParam(1);
	CString sResponse = ToUnicode(str).c_str();
	if(sResponse.Find(TEXT("0-4")) != -1)//无卡
	{
		m_pMgr->m_bRUIMSupported = false;
	}
	else if(sResponse.Find(TEXT("0-6")) != -1)//有卡
	{
		m_pMgr->m_bRUIMSupported = true;
	}
	else//无法判断,当有卡处理
	{
		m_pMgr->m_bRUIMSupported = true;
	}
}


//**************************************************
//+ZFNM
//**************************************************
CResponseZFNM::CResponseZFNM(CMobileManager* pMgr)
:CSimpleResponse("+ZFNM", pMgr)
{
}

void CResponseZFNM::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	m_pMgr->m_strZFNM = parser.GetParsedParam(1).c_str();
	m_pMgr->PostMessageToClient(WM_ZFNM,0,0);
}


//**************************************************
//+ZVMI 语音mail通知
//**************************************************
CResponseZVMI::CResponseZVMI(CMobileManager* pMgr)
:CSimpleResponse("+ZVMI", pMgr)
{
}

void CResponseZVMI::HandleAT()
{
	m_pMgr->PostMessageToClient(WM_VOICEMAIL_NOTIFY,0,0);
}



//**************************************************
//+CLCK 锁设备
//**************************************************
CResponseCLCK::CResponseCLCK(CMobileManager* pMgr)
:CSimpleResponse("+CLCK", pMgr)
{
}

void CResponseCLCK::HandleAT()
{
//	CComParser parser;
//	parser.ParseString(s_RxBuff);
//	int uimstate = atoi(parser.GetParsedParam(1).c_str());
}

//**************************************************
//+CME ERROR	错误报告
//**************************************************
CResponseCMEError::CResponseCMEError(CMobileManager* pMgr)
:CSimpleResponse("+CME ERROR", pMgr)
{
}

void CResponseCMEError::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>1)
	{
		int errCode = atoi(parser.GetParsedParam(1).c_str());
		m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR_REPORT
										+errCode);
	}
	else
	{
		m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR_REPORT);
	}
}

//**************************************************
//+CMS ERROR 短信错误报告
//**************************************************
CResponseCMSError::CResponseCMSError(CMobileManager* pMgr)
:CSimpleResponse("+CMS ERROR", pMgr)
{
}

void CResponseCMSError::HandleAT()
{

	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>1)
	{
		
		if(208 == atoi(parser.GetParsedParam(1).c_str()))
		{
			m_pMgr->PostMessageToClient(WM_SMS_FULL, 0, 0);
		}
		
		m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR_REPORT
										+atoi(parser.GetParsedParam(1).c_str()));
/*		
		if((310 == atoi(parser.GetParsedParam(1)))&&!m_pMgr->noSimHasReport)
		{
			m_pMgr->noSimHasReport = true;
			m_pMgr->PostMessageToClient(WM_NO_SIM, 0, 0);
		}
*/

	}
	else
	{
		m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR_REPORT);
	}
}


//**************************************************
//ERROR 普通错误
//**************************************************
CResponseError::CResponseError(CMobileManager* pMgr)
:CSimpleResponse("ERROR", pMgr)
{
}

void CResponseError::HandleAT()
{
	m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR);
}

//**************************************************
//OK 成功返回
//**************************************************
CResponseOK::CResponseOK(CMobileManager* pMgr)
:CSimpleResponse("OK", pMgr)
{
}

void CResponseOK::HandleAT()
{
	m_pMgr->m_SerialPort.SetReturnCode(RETURN_OK);
}

//**************************************************
//	+CMT 收到短消息立即显示
//	注意:这里由于事先已经设置短信通知格式为 +CMTI,
//		 所以正常情况下不应该出现 +CMT的短信通知,
//	     但由于5105嵌入端的bug, 在+CMGR时有时会得到
//		 此应答，在此情形下，将+CMT当作错误处理
//**************************************************
CResponseCMT::CResponseCMT(CMobileManager* pMgr)
:CATResponse("+CMT", pMgr)
{
}

bool CResponseCMT::OnRecvChar(char cRecv)
{
	if(RecvLine(cRecv))
	{
		//简单地当作错误处理
		m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR);
		return true;
	}
	else
		return false;
}


//+CPIN
CResponseCPIN::CResponseCPIN(CMobileManager* pMgr)
:CSimpleResponse("+CPIN", pMgr)
{
}

void CResponseCPIN::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>1)
	{
		std::string str = parser.GetParsedParam(1);
		m_pMgr->m_strPINResponse = ToUnicode(str).c_str();
	}
}

/*
CResponseOCPIN::CResponseOCPIN(CMobileManager* pMgr)
:CSimpleResponse("^CPIN", pMgr)
{
}

void CResponseOCPIN::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>1)
	{
		TRACE(TEXT("CResponseOCPIN::HandleAT:  %s"),parser.GetParsedParam(1).c_str());
		std::string str = parser.GetParsedParam(1);
		m_pMgr->m_strPINResponse = ToUnicode(str).c_str();

		m_pMgr->m_strPINResponse.MakeLower();
		int puk;
		int pin;
		if ( m_pMgr->m_strPINResponse == "ready" )
		{
			puk = atoi((parser.GetParsedParam(2).c_str() + 1));
			pin  = atoi(parser.GetParsedParam(3).c_str()); 
		}
		else
		{
			puk = atoi(parser.GetParsedParam(3).c_str());
			pin  = atoi(parser.GetParsedParam(4).c_str()); 
		}
		if (pin != 0)
		{
			m_pMgr->m_bpinpuk = true;
			m_pMgr->m_uremainingtime = pin;
		}
		else
		{
			m_pMgr->m_bpinpuk = false;
			m_pMgr->m_uremainingtime = puk;
		}
	}
}
*/

//+IPR
CResponseIPR::CResponseIPR(CMobileManager* pMgr)
:CSimpleResponse("*IPR", pMgr)
{
}
void CResponseIPR::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>=2)
		m_pMgr->m_nCurBaud = atoi(parser.GetParsedParam(1).c_str());
}

//+ZCRDN 呼叫转移
CResponseZCRDN::CResponseZCRDN(CMobileManager* pMgr)
:CSimpleResponse("+ZCRDN", pMgr)
{
}

void CResponseZCRDN::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
//	m_pMgr->GetCallManager().OnCallForward(parser.GetParsedParam(6));
}

//+TINFO 绑定标志
CResponseTINFO::CResponseTINFO(CMobileManager* pMgr)
:CSimpleResponse("+TINFO", pMgr)
{
}

void CResponseTINFO::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount()>1)
	{
		m_pMgr->m_nBindTag = atoi(parser.GetParsedParam(1).c_str());
	}
}

//+CCWA 呼叫等待
CResponseCCWA::CResponseCCWA(CMobileManager* pMgr)
:CSimpleResponse("+CCWA", pMgr)
{
}

void CResponseCCWA::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	
//	m_pMgr->GetCallManager().OnCallWating(parser.GetParsedParam(1));
}

/*jiangguochao added 20070115*/
//BUSY呼叫对象忙
CResponseBusyNow::CResponseBusyNow(CMobileManager* pMgr)
:CSimpleResponse("BUSY", pMgr)
{
}

void CResponseBusyNow::HandleAT()
{
	m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR);
	m_pMgr->GetCallManager().OnCallEnd(2);
}

/*jiangguochao added 20070115*/
//BUSY呼叫对象忙
CResponseNoCarrier::CResponseNoCarrier(CMobileManager* pMgr)
:CSimpleResponse("NO CARRIER", pMgr)
{
}

void CResponseNoCarrier::HandleAT()
{
	TRACE(TEXT("CResponseNoCarrier::HandleAT:> OnCallEnd"));
//	m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR);
//	m_pMgr->GetCallManager().OnCallEnd(1);
}

//**************************************************
//Connection established CONNECT //jiangguochao added 20070115
//**************************************************
CResponseConnected::CResponseConnected(CMobileManager* pMgr)
:CSimpleResponse("CONNECT", pMgr)
{
}

void CResponseConnected::HandleAT()
{
}

//**************************************************
//Connection completion timeout NO ANSWER//jiangguochao added 20070115
//**************************************************
CResponseNoAnswer::CResponseNoAnswer(CMobileManager* pMgr)
:CSimpleResponse("NO ANSWER", pMgr)
{
}

void CResponseNoAnswer::HandleAT()
{
	TRACE(TEXT("CResponseNoAnswer::HandleAT:> OnCallEnd"));
	m_pMgr->m_SerialPort.SetReturnCode(RETURN_ERROR);
	m_pMgr->GetCallManager().OnCallEnd(0);
}

CResponseCOPS::CResponseCOPS(CMobileManager* pMgr)
:CSimpleResponse("+COPS", pMgr)
{
}

void CResponseCOPS::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	int count = 0;
	count = parser.GetParsedParamCount();
	CString netName = "";

	if(count >2)
	{
		std::string str = parser.GetParsedParam(1);
		CString mode = ToUnicode(str).c_str();
		if((("0"==mode)||("1"==mode)||("2"==mode)||("3"==mode)||("4"==mode)))
		{
			m_pMgr->m_IsRegister = true;	
			if ( "0" == mode )
			{
				m_pMgr->PostMessageToClient(WM_SET_AUTO, 0, 0);
			}
			else if ( "1" == mode )
			{
				m_pMgr->PostMessageToClient(WM_SET_AUTO, 1, 0);
			}
				
			if(count >= 4)
			{
				std::string str = parser.GetParsedParam(3);
				m_pMgr->m_strCurNet = ToUnicode(str).c_str();
				m_pMgr->m_CurACT = atoi(parser.GetParsedParam(4).c_str());
				m_pMgr->m_CurFormat= atoi(parser.GetParsedParam(2).c_str());

				///通知界面
				m_pMgr->PostMessageToClient(WM_SHOW_NETWORK, 0, 0);
			}
			return;
		}
		
		for(int i = 1; i < count; i++)
		{
			CString NetworkInfo;
			std::string str = parser.GetParsedParam(i);
			NetworkInfo = ToUnicode(str).c_str();  
			
			if ( ",(0" == NetworkInfo )
			{	
				return;
			}
			else
			{
				m_pMgr->m_AllNet.InsertAt(i-1, NetworkInfo);
			}
			
		}
		if(m_pMgr->g_pfnDwonLoadVerCallBack != NULL)
		m_pMgr->g_pfnDwonLoadVerCallBack(NULL);
	}
	else if ( 2 == count )
	{
		m_pMgr->m_IsRegister = false;
	}
}

CResponseCPOL::CResponseCPOL(CMobileManager* pMgr)
:CSimpleResponse("+CPOL", pMgr)
{
}

void CResponseCPOL::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount() >= 4)
	{
		std::string strCurNet = parser.GetParsedParam(3);
		m_pMgr->m_strCurNet = ToUnicode(strCurNet).c_str();
	}
}

CResponseSWRV::CResponseSWRV(CMobileManager* pMgr)
:CSimpleResponse("*SWRV", pMgr)
{
}

void CResponseSWRV::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	
	std::string strSwrv;
	strSwrv = parser.GetParsedParam(1);


	m_pMgr->m_strSWRV = ToUnicode(strSwrv).c_str();
}

CResponsePPP::CResponsePPP(CMobileManager* pMgr)
:CSimpleResponse("*PPP", pMgr)
{
}

void CResponsePPP::HandleAT()
{
	CComParser parser;
	parser.ParseString(s_RxBuff);
	if(parser.GetParsedParamCount() >= 1)
	{
		int nSecond = atoi(parser.GetParsedParam(1).c_str()) ;
		m_pMgr->m_nPPPSecond = nSecond;
	}
}

CResponseATOK::CResponseATOK(CMobileManager* pMgr)
:CSimpleResponse("+ATAOK", pMgr)
{
}

void CResponseATOK::HandleAT()
{
	m_pMgr->GetCallManager().OnCallContinue();
}
