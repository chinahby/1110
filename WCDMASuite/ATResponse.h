#ifndef AFX_ATRESPONSE_H__688C9FEF_BDFE_4C33_8EE8_5F7498089D1C__INCLUDED_
#define AFX_ATRESPONSE_H__688C9FEF_BDFE_4C33_8EE8_5F7498089D1C__INCLUDED_

class CMobileManager;
//处理AT的回应类族,使用 Status 设计模式

class CATResponse  
{
public:
	CATResponse(const CString& strName, CMobileManager* pMgr);
	virtual ~CATResponse();

	bool MatchByName(const CString& name)const;

	virtual bool OnRecvChar(char cRecv) = 0;

	static void ResetRxBuf();
protected:
	CString m_strName;
	CMobileManager* m_pMgr;
protected:
	bool RecvLine(char cRecv);
protected:
	enum{RXBUFF_SIZE = 102400};
	
	static char s_RxBuff[RXBUFF_SIZE];
	static int  s_RxBuffSize;
};

//简单命令回应，命令以'\r' 或 '\n' 或 "\r\n"结束
class CSimpleResponse: public CATResponse
{
public:
	CSimpleResponse(const CString& strName, CMobileManager* pMgr);
protected:
	virtual bool OnRecvChar(char cRecv);

	virtual void HandleAT() = 0;
};

//基本的AT处理状态
class CNormalResponse: public CATResponse
{
public:
	CNormalResponse();
	CString GetResponseString()const;
protected:
	virtual bool OnRecvChar(char cRecv);
};

/*
//+CDS
class CResponseCDS: public CSimpleResponse
{
public:
	CResponseCDS(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};*/


//+CSQ
class CResponseCSQ: public CSimpleResponse
{
public:
	CResponseCSQ(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//^HDRCSQ		//ADD BY LAB686 2009/10/29
class CResponseHDRCSQ: public CSimpleResponse
{
public:
	CResponseHDRCSQ(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//^PREFMODE		//ADD BY LAB686 2009/11/12
class CResponsePREFMODE: public CSimpleResponse
{
public:
	CResponsePREFMODE(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//^MODE		//ADD BY LAB686 2009/11/12
class CResponseMODE: public CSimpleResponse
{
public:
	CResponseMODE(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CREG  网络注册 
class CResponseCREG: public CSimpleResponse
{
public:
	CResponseCREG(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CGATT
class CResponseCGATT: public CSimpleResponse
{
public:
	CResponseCGATT(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//*EDGE
class CResponseEDGE: public CSimpleResponse
{
public:
	CResponseEDGE(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//*IMEI
class CResponseIMEI: public CSimpleResponse
{
public:
	CResponseIMEI(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};
//*SOFTWV
class CResponseSimLock: public CSimpleResponse
{
public:
	CResponseSimLock(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//^CONN	// CHANGE BY LAB686 20091029
class CResponseCONN: public CSimpleResponse
{
public:
	CResponseCONN(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};


//+RING
class CResponseRING: public CSimpleResponse
{
public:
	CResponseRING(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CLIP
class CResponseCLIP: public CSimpleResponse
{
public:
	CResponseCLIP(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CPAS
class CResponseCPAS: public CSimpleResponse
{
public:
	CResponseCPAS(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//^CEND
class CResponseCEND: public CSimpleResponse
{
public:
	CResponseCEND(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CIND
class CResponseCIND: public CSimpleResponse
{
public:
	CResponseCIND(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};
//+VGR
class CResponseCLVL: public CSimpleResponse
{
public:
	CResponseCLVL(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

/*+ZMGF
class CResponseZMGF: public CSimpleResponse
{
public:
	CResponseZMGF(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};
*/

/**************************   短信***************************/
//+CMTI
class CResponseCMTI: public CSimpleResponse
{
public:
	CResponseCMTI(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//FULL
class CResponseSMSFULL: public CSimpleResponse
{
public:
	CResponseSMSFULL(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

// 新收到的短信状态报告+CDSI
class CResponseSMSCDSI: public CSimpleResponse			// ADD BY LAB686 20091103
{
public:
	CResponseSMSCDSI(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

// 10. 新短信状态报告直接上报指示^HCDS
class CResponseSMSHCDS: public CSimpleResponse 			// ADD BY LAB686 20091103
{
public:
	CResponseSMSHCDS(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//短信发送成功上报指示+CMGS:
class CResponseSMSCMGS: public CSimpleResponse 	
{
public:
	CResponseSMSCMGS(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//短信发送失败上报指示+CMS ERROR:
class CResponseSMSERROR: public CSimpleResponse
{
public:
	CResponseSMSERROR(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//> no used
//SIM LOck
class CResponseSIMER: public CSimpleResponse
{
public:
	CResponseSIMER(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CMGR
class CResponseCMGR: public CATResponse
{
public:
	CResponseCMGR(CMobileManager* pMgr);
	~CResponseCMGR();
protected:
	virtual bool OnRecvChar(char cRecv);
private:
	bool ReadContent(char cRecv);
	void SMSReadingOver();
	void Clean();
	void ExtractPDU();
private:
	int	 m_PDULen;
	char* m_PDUBytes;
	int  m_strtype;
};

//+CPMS
class CResponseCPMS: public CSimpleResponse
{
public:
	CResponseCPMS(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

class CThreadLoadPB;
//+CPBW
class CResponseCPBW: public CSimpleResponse
{
	CThreadLoadPB* m_ThreadLoadPB;
public:
	CResponseCPBW(CMobileManager* pMgr);
	void SetLoadingThread(CThreadLoadPB* thread);
protected:
	virtual void HandleAT();
};

//+CPBR
class CResponseCPBR: public CSimpleResponse
{
	CThreadLoadPB* m_ThreadLoadPB;
public:
	CResponseCPBR(CMobileManager* pMgr);
	void SetLoadingThread(CThreadLoadPB* thread);
protected:
	virtual void HandleAT();
	CString ExtractUnicodeName(const CString& Unicode);
};

/*
//+CNUM
class CResponseCNUM: public CSimpleResponse
{
public:
	CResponseCNUM(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};
*/


//+CCFC
class CResponseCCFC: public CSimpleResponse
{
public:
	CResponseCCFC(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};


//+CSCA
class CResponseCSCA: public CSimpleResponse
{
public:
	CResponseCSCA(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};


//S/W VER: 
class CResponseATI3: public CSimpleResponse
{
public:
	CResponseATI3(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+ZDPB
class CResponseZDPB: public CSimpleResponse
{
public:
	CResponseZDPB(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+ZFNM
class CResponseZFNM: public CSimpleResponse
{
public:
	CResponseZFNM(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};


//+ZVMI
class CResponseZVMI: public CSimpleResponse
{
public:
	CResponseZVMI(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};



//+CLCK
class CResponseCLCK: public CSimpleResponse
{
public:
	CResponseCLCK(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CME ERROR
class CResponseCMEError: public CSimpleResponse
{
public:
	CResponseCMEError(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CMS ERROR
class CResponseCMSError: public CSimpleResponse
{
public:
	CResponseCMSError(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//ERROR
class CResponseError: public CSimpleResponse
{
public:
	CResponseError(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//OK
class CResponseOK: public CSimpleResponse
{
public:
	CResponseOK(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CMT
class CResponseCMT: public CATResponse
{
public:
	CResponseCMT(CMobileManager* pMgr);
protected:
	virtual bool OnRecvChar(char cRecv);
};

//+CPIN
class CResponseCPIN: public CSimpleResponse
{
public:
	CResponseCPIN(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+ZCORG
class CResponseORIG: public CSimpleResponse
{
public:
	CResponseORIG(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+IPR
class CResponseIPR: public CSimpleResponse
{
public:
	CResponseIPR(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+ZCRDN 呼叫转移
class CResponseZCRDN: public CSimpleResponse
{
public:
	CResponseZCRDN(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+TINFO 绑定标志
class CResponseTINFO: public CSimpleResponse
{
public:
	CResponseTINFO(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

//+CCWA 呼叫等待
class CResponseCCWA: public CSimpleResponse
{
public:
	CResponseCCWA(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

/*jiangguochao added 20071015*/
//BUSY呼叫对象忙
class CResponseBusyNow: public CSimpleResponse
{
public:
	CResponseBusyNow(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

/*jiangguochao added 20071015*/
//没有建立连接NO CARRIER
class CResponseNoCarrier: public CSimpleResponse
{
public:
	CResponseNoCarrier(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

/*jiangguochao added 20071015*/
//Connection established CONNECT
class CResponseConnected: public CSimpleResponse
{
public:
	CResponseConnected(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

/*jiangguochao added 20071015*/
//Connection completion timeout NO ANSWER
class CResponseNoAnswer: public CSimpleResponse
{
public:
	CResponseNoAnswer(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

class CResponseCPOL: public CSimpleResponse
{
public:
	CResponseCPOL(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};
class CResponseCOPS: public CSimpleResponse
{
public:
	CResponseCOPS(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

class CResponseSWRV: public CSimpleResponse
{
public:
	CResponseSWRV(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

class CResponsePPP: public CSimpleResponse
{
public:
	CResponsePPP(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

class CResponseATOK: public CSimpleResponse
{
public:
	CResponseATOK(CMobileManager* pMgr);
protected:
	virtual void HandleAT();
};

#endif // !defined(AFX_ATRESPONSE_H__688C9FEF_BDFE_4C33_8EE8_5F7498089D1C__INCLUDED_)
