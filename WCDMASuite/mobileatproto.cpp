#include "stdafx.h"
#include "MobileATProto.h"
#include "Util.h"
//#include "GlobalConst.h"
#include "PDU.h"
#include "CallCmdPacket.h"
#include "CHangupCmdPacket.h"
#include "AcceptCmdPacket.h"
#include "SetBaudCmdpacket.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMobilePacket* CMobileATProto::GeneratePacket(const char* strCmd, size_t strSize)
{
	CMobilePacket* packet = new CMobilePacket(strCmd, strSize);
	return packet;
}

static int GetKeyVoiceFreq(char cKey)
{
	static const char key[] = 
	{
		'0','1','2','3','4','5','6','7','8','9',
		'*','#',
		'C',//Clear
		'D',//Dial
		'H',//Hangup
		'R',//Recall
		'+'
	};

	for(int i=0;i<sizeof(key);++i)
	{
		if(key[i] == cKey)
		{
			return 400+i*60;
		}
	}

	return 1700;
}

CMobilePacket* CMobileATProto::KeyInput(char cKey)
{

	CString strCmd;
	strCmd.Format(TEXT("at+ztone=1,1,%d,2,1\r"), GetKeyVoiceFreq(cKey));//jiangguochao need change
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(), str.size());
}


CMobilePacket* CMobileATProto::GetVolumn()
{
	static const char strCmd[] = "AT+CLVL?\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::SetVolumn(int nVolumn)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CLVL=%d\r"), nVolumn);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());
}

CMobilePacket* CMobileATProto::SetEcho(bool bOn)
{
	///设置回显
	std::string strCmd = bOn ? "ATE1\r" : "ATE0\r";

	return GeneratePacket(strCmd.c_str(), strCmd.size());
}

CMobilePacket* CMobileATProto::TestSimLock()				// MODIFY BY LAB686 // 
{
	///测试SIM 锁定
	std::string strCmd = "AT+CLCK=\"SC\",2\r";

	return GeneratePacket(strCmd.c_str(), strCmd.size());		// return 0: 未激活1:激活
}

CMobilePacket* CMobileATProto::SETSimLock(int mode, const CString& strPinCode)				// MODIFY BY LAB686 // 
{
	// 设置SIM 锁状态
	 CString strCmd;
	strCmd.Format(TEXT("AT+CLCK=\"SC\",%d,\"%s\"\r"), mode, strPinCode);

	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());		// return 0: 未激活1:激活
}

CMobilePacket* CMobileATProto::SetCmee()					// ok lab686
{
	std::string strCmd = "AT+CMEE=2\r";
	return GeneratePacket(strCmd.c_str(), strCmd.size());
}

CMobilePacket* CMobileATProto::SetCreg()
{
	std::string  strCmd = "AT+CREG=2\r";
	return GeneratePacket(strCmd.c_str(), strCmd.size());
}


CMobilePacket* CMobileATProto::QueryIMEI()	// OK LAB686
{
	std::string  strCmd = "AT+CIMI\r";
	return GeneratePacket(strCmd.c_str(), strCmd.size());
}

CMobilePacket* CMobileATProto::QueryCreg()
{
	std::string  strCmd = "AT+CREG?\r";
	return GeneratePacket(strCmd.c_str(), strCmd.size());
}

CMobilePacket* CMobileATProto::QueryGprsAttach()
{
	std::string  strCmd = "AT+CGATT?\r";
	return GeneratePacket(strCmd.c_str(), strCmd.size());
}

CMobilePacket* CMobileATProto::QueryEDGEAttach()
{
	std::string strCmd = "AT*EDGE\r";
	return GeneratePacket(strCmd.c_str(), strCmd.size());
}

CMobilePacket* CMobileATProto::QueryCallEnd()
{
	std::string strCmd = "AT+CIND?\r";
	return GeneratePacket(strCmd.c_str(), strCmd.size());
}


/*
CMobilePacket* CMobileATProto::CheckSimExist()
{
	CString strCmd = "AT+CSIM=?\r";
	return GeneratePacket(strCmd, strCmd.GetLength());
}
*/

/*
CMobilePacket* CMobileATProto::LocalPhoneNum()
{
	///查询本机电话
 	static const char strCmd[] = "AT+CNUM\r";

	CMobilePacket* packet = GeneratePacket(strCmd, sizeof(strCmd)-1);
	
	return packet;
}


CMobilePacket* CMobileATProto::SetLocalPhone(const CString& strLocalPhone)
{
	CString strCmd;
	strCmd.Format("AT+CNUM=%s\r", strLocalPhone);

	return GeneratePacket(strCmd, strCmd.GetLength());
}
*/

CMobilePacket* CMobileATProto::SetCMGF()				// modify by lab686 20091030
{
	/// 设置短信格式命令
 	static const char strCmd[] = "AT+CMGF=0\r";

	CMobilePacket* packet = GeneratePacket(strCmd,sizeof(strCmd)-1);
	
	return packet;
}


CMobilePacket* CMobileATProto::SetCharset()
{
	///设置字符集
 	static const char strCmd[] = "AT+CSCS=\"UCS2\"\r";

	CMobilePacket* packet = GeneratePacket(strCmd, sizeof(strCmd)-1);
	
	return packet;
}


CMobilePacket* CMobileATProto::CscaPhoneNum()
{
	///查询短消息服务中心
 	static const char strCmd[] = "AT+CSCA?\r";

	CMobilePacket* packet = GeneratePacket(strCmd, sizeof(strCmd)-1);
	
	return packet;
}


CMobilePacket* CMobileATProto::LocalVersion()
{
	///查询软件版本
 	static const char strCmd[] = "ATI3\r";

	CMobilePacket* packet = GeneratePacket(strCmd, sizeof(strCmd)-1);
	
	return packet;
}


CMobilePacket* CMobileATProto::SetBaudrate(int iRate)
{
	///设置波特率
 	
	return new CSetBaudCmdpacket(iRate);
}

CMobilePacket* CMobileATProto::GetBaudrate()
{
	///获取当前波特率
 	static const char strCmd[] = "AT*IPR?\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::QueryBaud()
{
	///查询模块的波特率
 	static const char strCmd[] = "AT*IPR?\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::TestComATOK()				// ok lab686 
{
	///查询软件版本
 	static const char strCmd[] = "AT\r";

	CMobilePacket* packet = GeneratePacket(strCmd, sizeof(strCmd)-1);
	
	return packet;
}

CMobilePacket* CMobileATProto::SetMode(int mode)
{
 	CString strCmd;

	strCmd.Format(TEXT("AT^PREFMODE=%d\r"), mode);

	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());
}

CMobilePacket* CMobileATProto::QueryMode()
{
 	CString strCmd;

	strCmd.Format(TEXT("AT^PREFMODE=?\r"));

	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());
}
CMobilePacket* CMobileATProto::CheckSignal()
{
	///查询信号量
 	static const char strCmd[] = "AT+CSQ\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::CheckHDRSignal()				// ok lab686
{
	///查询信号量
 	static const char strCmd[] = "AT^HDRCSQ\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::CheckSYSINFO()
{
 	static const char strCmd[] = "AT^SYSINFO\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::CheckCPIN()
{
	///测试CPIN,返回error表示UIM不存在或不能识别
 	static const char strCmd[] = "AT+CPIN?\r";//jiangguochao need change

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::CheckMIC()
{
	///查询MIC静音
 	static const char strCmd[] = "AT+CMUT=?\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::SetMICMute(bool bSet)
{
	///设置MIC静音
 	CString strCmd;
	if(bSet)
		strCmd.Format(TEXT("AT+CMUT=1\r"));
	else 
		strCmd.Format(TEXT("AT+CMUT=0\r"));

	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());
}

CMobilePacket* CMobileATProto::SetDialShow(bool bShow)
{
	///来电显示
 	CString strCmd;
	if(bShow)
		strCmd.Format(TEXT("AT+CLIP=%d\r"), 1);
	else
		strCmd.Format(TEXT("AT+CLIP=%d\r"), 0);

	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());
}


CMobilePacket* CMobileATProto::SetSMSPrompt()					// modify by lab686
{
	///设置新短信提示
 	//static const char strCmd[] = "AT+CNMI=2,1,0,1,0\r";
 	static const char strCmd[] = "AT+CNMI=1,1,0,0,0\r";				// 电信用的AT+CNMI=1,1,1,1,0.	

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::SetSMSPraCmd(int fmt)
{
	//设置要发送的短信参数命令
	CString strCmd;
	strCmd.Format(TEXT("AT^HSMSSS=1,0,%d,0\r"), fmt);

	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(), str.size());
}

CMobilePacket* CMobileATProto::SetPhoneBookEnable()
{
	////允许电话本操作
 	static const char strCmd[] = "AT+ZAIP=0\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::DialHangUp()
{
	return new CHangupCmdPacket();
}

CMobilePacket* CMobileATProto::SMSRead(int nID)					// modify by lab686
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CMGR=%d,1\r"), nID);//+CMGR

	std::string str = ToMultiBytes(LPCTSTR(strCmd));
									
	CMobilePacket* packet = GeneratePacket(str.c_str(), str.size());
	return packet;
}

CMobilePacket* CMobileATProto::SMSSave(const CString& strPhoneNum, const CString& strContent)
{									
	static const char chCtrlZ = 0x1a;
	static const char chCR = 0x0d;

	CString strCmd;
	strCmd.Format(TEXT("AT^HCMGW=\"%s\""), strPhoneNum);
	strCmd = strCmd + "0,0,0,0,1,1"	;									// modify by lab686 20091031
	strCmd += chCR;
	strCmd += strContent;
	strCmd += chCtrlZ;

	std::string str = ToMultiBytes(LPCTSTR(strCmd));
									
	CMobilePacket* packet = GeneratePacket(str.c_str(), str.size());

	return packet;
}

CMobilePacket* CMobileATProto::SMSDelete(int nID)					// lab686
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CMGD=%d,0\r"), nID);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	
	return GeneratePacket(str.c_str(), str.size());;

}

CMobilePacket* CMobileATProto::SMSDeleteAll()						// lab686
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CMGD=%d,%d\r"), 1,4);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	
	return GeneratePacket(str.c_str(), str.size());;

}

CMobilePacket* CMobileATProto::SMSBoxSelect(const CString& strType)	// lab686
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CPMS=\"%s\",\"%s\",\"%s\"\r"), strType,strType,strType);		// "SM"，表示SIM卡/ "ME"，表示NV
	std::string str = ToMultiBytes(LPCTSTR(strCmd));								

	return GeneratePacket(str.c_str(), str.size());
}

CMobilePacket* CMobileATProto::PhoneBookType()
{
	std::string strCmd = "AT+CPBW=?\r";
									
	return GeneratePacket(strCmd.c_str(), strCmd.size());

}

/*
CMobilePacket* CMobileATProto::PhoneBookParam()
{
	static const char strCmd[] = "AT+CPBR=?\r";
									
	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}
*/

CMobilePacket* CMobileATProto::PhoneBookLoad(int nMinID, int nMaxID)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CPBR=%d,%d\r"), nMinID, nMaxID);

	std::string str = ToMultiBytes(LPCTSTR(strCmd));
									
	return GeneratePacket(str.c_str(), str.size());
}

CMobilePacket* CMobileATProto::PhoneBookDelete(int nID)
{
	return PhoneBookModify(nID, " ", " ");
}

CMobilePacket* CMobileATProto::PhoneBookSelect(const CString& strType)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CPBS=\"%s\"\r"), strType);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	
	return GeneratePacket(str.c_str(), str.size());
}

char Byte2Char(BYTE b);

CMobilePacket* CMobileATProto::PhoneBookModify
(
	int nID, 
	const CString& strPhoneNum, 
	const CString& strName
)
{

	//格式 AT+CPBW=ID,"号码",129,\r内容

	char strCmd[400]={0};
	
	if((" " == strPhoneNum)&&(" " == strName))
	{
		sprintf(strCmd,"AT+CPBW=%d\r",nID);
		return GeneratePacket((char*)strCmd,strlen(strCmd));
	}
	else
	{
		sprintf(strCmd,"AT+CPBW=%d,\"%s\",,\"",nID,ToMultiBytes((LPCTSTR)strPhoneNum).c_str());
	}
	
	int nCmdLen = 0;
	nCmdLen = strlen(strCmd);
	

	char* pName = strCmd+nCmdLen;

	CString tempStrName = strName;
	tempStrName.TrimLeft();
	tempStrName.TrimRight();
       if(tempStrName.IsEmpty())
	{
		strCmd[nCmdLen++] = '\"';
		strCmd[nCmdLen++] = '\r';
		return GeneratePacket((char*)strCmd, nCmdLen);
	}
	else
	{
		//if(GetStringType(strName) == STR_DOUBLE_BYTE)
		{
			
			vector<BYTE>data;
			data.reserve(500);

			//Unicode 的内容格式为 0X80 uniocde			
			WCHAR unicode[400] = {0x0080,0};
			//std::string stra = ToMultiBytes(LPCTSTR(tempStrName));
			//int nCharCount = ASCIIToUnicode(stra.c_str(), stra.size(), unicode, sizeof(unicode));
			int nCharCount = tempStrName.GetLength();
			_tcscpy(unicode,LPCTSTR(tempStrName))  ;
			data.push_back((BYTE)(nCharCount*2));

			for(int i=0; i<nCharCount; ++i)
			{
				WCHAR w = unicode[i];
				data.push_back((w&0xFF00)>>8);
				data.push_back(w&0x00FF);
			}

			CString str;
			for(size_t j=0; j<data.size(); ++j)
			{
				BYTE b = data[j];
				BYTE l = b&0X0F;
				BYTE h = (b&0XF0)>>4;
				str += Byte2Char(h);
				str += Byte2Char(l);
			}

			int nNameBytes = nCharCount*2;

			int count = nNameBytes*2;
			str = str.Right(count);

			
			memcpy(pName, ToMultiBytes(LPCTSTR(str)).c_str(), count);
			
			nCmdLen += count;
			
			strCmd[nCmdLen++] = '\"';
			strCmd[nCmdLen++] = '\r';			
		}
	/*	else
		{
			//ASCII 的内容格式为 name 0X0A
			int nNameBytes = strName.GetLength();
			memcpy(pName, ToMultiBytes((LPCTSTR)strName).c_str(), nNameBytes);
			nCmdLen += nNameBytes;
			strCmd[nCmdLen++] = '\"';
			strCmd[nCmdLen++] = '\r';
		}*/
	}
	
	return GeneratePacket((char*)strCmd, nCmdLen);
}

//判断当前软件为有卡或无卡软件,
//目前的实现方法为：
//通过发送AT+ZDPB=?，返回+ZDPB: (0-6)为有卡，
//返回+ZDPB: (0-4)为无卡
CMobilePacket* CMobileATProto::CheckRUIMSurport()
{
	static const char strCmd[] = "AT+ZDPB=?\r";//jiangguochao need change 20070115

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}


CMobilePacket* CMobileATProto::ReadAOLock()
{
	static const char strCmd[] = "AT+CLCK=\"AO\",2\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::SetAOLock(bool bLock)
{
	static const char strCmd1[] = "AT+CLCK=\"AO\",1,0000\r";
	static const char strCmd0[] = "AT+CLCK=\"AO\",0,0000\r";

	if(bLock)
		return GeneratePacket(strCmd1, sizeof(strCmd1)-1);
	else
		return GeneratePacket(strCmd0, sizeof(strCmd0)-1);
}

CMobilePacket* CMobileATProto::ReadAILock()
{
	static const char strCmd[] = "AT+CLCK=\"AI\",2\r";

	return GeneratePacket(strCmd, sizeof(strCmd)-1);
}

CMobilePacket* CMobileATProto::SetAILock(bool bLock)
{
	static const char strCmd1[] = "AT+CLCK=\"AI\",1,0000\r";
	static const char strCmd0[] = "AT+CLCK=\"AI\",0,0000\r";

	if(bLock)
		return GeneratePacket(strCmd1, sizeof(strCmd1)-1);
	else
		return GeneratePacket(strCmd0, sizeof(strCmd0)-1);
}


//发送键盘dtmf信号
CMobilePacket* CMobileATProto::SendDtmf(const CString& strKey, size_t callnum)	// modify by lab686 
{
	CString strCmd;
	CString cstr;
	cstr.Format(_T("%d"), callnum);
	strCmd = "AT^DTMF=";
	strCmd += cstr;
	strCmd += ",";
	strCmd += strKey;
	strCmd += "\r";

	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());
}

// 设置语音通道端口
CMobilePacket* CMobileATProto::SendDDSETEX(size_t comnumber)		// ok 		// modify by lab686 
{
	CString strCmd;
	CString cstr;
	cstr.Format(_T("%d"), comnumber);
	strCmd = "AT^DDSETEX=";
	strCmd += cstr;
	strCmd += "\r";

	std::string str = ToMultiBytes(LPCTSTR(strCmd));

	return GeneratePacket(str.c_str(), str.size());
}

// 设置语音通道端口
CMobilePacket* CMobileATProto::SetCVOICE(size_t comnumber)				// modify by lab686 
{

	CString cstr;
	cstr.Format(TEXT("AT^CVOICE=%d\r"),comnumber);		// 一般设置为0

	std::string str = ToMultiBytes(LPCTSTR(cstr));

	return GeneratePacket(str.c_str(), str.size());
}

//呼出
CMobilePacket* CMobileATProto::DialCallOut(const CString& strPhoneNum, CCallManager* pCallMgr)
{
	std::string str = ToMultiBytes(LPCTSTR(strPhoneNum)); 
	return new CCallCmdPacket(str, pCallMgr);
}

//呼叫接听
CMobilePacket* CMobileATProto::DialAccept(CCallManager* pCallMgr)
{
	return new CAcceptCmdPacket(pCallMgr);
}

CMobilePacket* CMobileATProto::SelectRing(BYTE nMusicIndex, BYTE deviceNo)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+ZCDM=%d,%d\r"), nMusicIndex, deviceNo);//jiangguochao 20070115 need development

	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::InputPIN1(const CString& strPIN)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CPIN=\"%s\"\r"), strPIN);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::InputPUK1(const CString& strPUK, const CString& strNewPIN)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CPIN=\"%s\",\"%s\"\r"), strPUK, strNewPIN);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::QueryLockTag()
{
	std::string strCmd ="AT+TINFO\r";//jiangguochao 20070115 need development
	return GeneratePacket(strCmd.c_str(),strCmd.size());
}

CMobilePacket* CMobileATProto::SendFlash(const char* strNum)
{
	CString strCmd;
	if(strNum)
		strCmd.Format(TEXT("AT+ZFLSH=%s\r"), strNum);
	else
		strCmd = "AT+ZFLSH\r";
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(), str.size());
}

char Byte2Char(BYTE b)
{
	if(b>=0 && b<=9)
		return b+'0';
	else if(b>9 && b<16)
		return b-10+'A';
	else 
		ASSERT(0);
	return '0';
}

BYTE HexChar2Byte(char b)
{
	if (b >= '0' && b <= '9')
		return b -'0';
	else if (b > 'a' && b< 'f')
		return b - 'a' + 10;
	else if (b > 'A' && b< 'F')
		return b - 'A' + 10;
	else
		ASSERT(0);
	return '0';
}

CString Byte2HexString(const vector<BYTE>& bytes)
{
	CString str;
	for(size_t i=0; i<bytes.size(); ++i)
	{
		BYTE b = bytes[i];
		BYTE l = b&0X0F;
		BYTE h = (b&0XF0)>>4;
		str += Byte2Char(h);
		str += Byte2Char(l);
	}

	return str;
}

CMobilePacket* CMobileATProto::SendPDUSMS
(
	const CString& SMSCenter, 
	const CString& strPhoneNum, 
	const CString& strText, 
	CMobilePacket*& cmd2
)
{
	
	PDU::PDUOut pdu(ToMultiBytes(LPCTSTR(strPhoneNum)), 
		ToMultiBytes(LPCTSTR(SMSCenter)), ToMultiBytes(LPCTSTR(strText)));
	vector<BYTE> bytes = pdu.Encode(0);

	CString strPDU = Byte2HexString(bytes);
	TCHAR ch = 0x1a;
	strPDU += ch;
	CString cmd;
	//cmd.Format("AT+CMGS=%d\r", pdu.DataLength(), strPDU);
	cmd.Format(TEXT("AT+CMGS=%d\r"), pdu.DataLength());
	std::string str1 = ToMultiBytes(LPCTSTR(strPDU));
	cmd2 = GeneratePacket(str1.c_str(), str1.size());
	std::string str2 = ToMultiBytes(LPCTSTR(cmd));
	return GeneratePacket(str2.c_str(), str2.size());
		
}


CMobilePacket* CMobileATProto::CallHold(int code)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CHLD=%d\r\n"), code);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::CallForwardSet(int reason, int mode, const char* strNum)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CCFC=%d,%d,\"+%s\",145\r\n"), reason,mode,strNum);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::CallForwardCancel(int reason, int mode)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CCFC=%d,%d\r\n"), reason,mode);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::RegisterNet(const CString& strNetName)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+COPS=1,2,\"%s\"\r"), strNetName);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::QueryCurNet()
{
	CString strCmd;
	strCmd.Format(TEXT("AT+COPS?\r"));
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::QueryAllNet()
{
	CString strCmd;
	strCmd.Format(TEXT("AT+COPS=?\r"));
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}


CMobilePacket* CMobileATProto::SetDevToUDisk()
{
	CString strCmd;
	strCmd.Format(TEXT("AT+PSUSBM=3\r"));
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::QueryState()
{
	std::string strCmd = "AT+CPAS\r";
	return GeneratePacket(strCmd.c_str(),strCmd.size());
}

CMobilePacket* CMobileATProto::QuerySWRV()
{
	CString strCmd;
	strCmd.Format(TEXT("AT*SWRV\r"));
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::QueryPPP()
{
	CString strCmd;
	strCmd.Format(TEXT("AT*PPP?\r"));
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::SetPCMSWitch(int nMode)
{
	CString strCmd;
	strCmd.Format(TEXT("AT*PCMSWITCH=%d\r"),nMode);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::SetApn(const CString& strApn)
{
	CString strCmd;
	strCmd.Format(TEXT("AT+CGDCONT=1,\"IP\",\"%s\"\r"),strApn);
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}

CMobilePacket* CMobileATProto::RSPDevice()
{
	CString strCmd;
	strCmd.Format(TEXT("AT*RSWD=1680\r"));
	std::string str = ToMultiBytes(LPCTSTR(strCmd));
	return GeneratePacket(str.c_str(),str.size());
}
