#ifndef FX_RASCLIENT_H__E30DDC47_AACE_4E00_810E_EFC69E3C91CC__INCLUDED_
#define AFX_RASCLIENT_H__E30DDC47_AACE_4E00_810E_EFC69E3C91CC__INCLUDED_

#include <Ras.h>
#include "infodialup.h"

struct IRasListener
{
	virtual void OnRasMsg(UINT unMsg,
					RASCONNSTATE rascs,
					DWORD dwError,
					DWORD dwExtendedError) = 0;
};

class CRasClient  
{
public:
	CRasClient();
	~CRasClient();

	bool Dial(const CString& strConnection,
			  const CString& strPhoneNum,
			  const CString& strUser,
			  const CString& strPasswrod);
	void Hangup();
	RASCONNSTATE GetStatus(DWORD* dwError = NULL)const;
	void SetListener(IRasListener* pListener);

	static CString GetStatusString(RASCONNSTATE status);
	static CString GetErrorString(DWORD dwError);
public:
	void BeginFluxCount(const CTime& tmBegin);
	const CInfoConnect& RefreshFluxCount();
	void EndFluxCount();
	const CInfoConnect& GetFluxCount();
	bool GetConnectState();//20070514
private:
	struct TConnectionStastics
	{
		DWORD   dwBytesXmited;	//发送的字节数
		DWORD   dwBytesRcved;	//接收的字节数
	};

	bool GetStatics(TConnectionStastics& info)const;
private:
	CRasClient(const CRasClient&);
	CRasClient& operator=(const CRasClient&);
private:
	void FireEvent( UINT unMsg,
					RASCONNSTATE rascs,
					DWORD dwError,
					DWORD dwExtendedError);
private:
	HRASCONN		m_hConnection;
	IRasListener*	m_pListener;
	friend class CRasMgr;

	CInfoConnect	m_ConnectInfo;
	DWORD			m_dwBeginTick;
};

class CRasEntry
{
public:
	CRasEntry(const CString& modemName, const CString& connectionName);
	bool IsExisting()const;
	bool Create(const CString& strLocalPhone, const CString& ipaddr,const CString& dns1, const CString& dns2,
				const CString& strUserName, const CString& strPassword);
private:
	CString m_DeveiceName;
	CString m_ConnectionName;
};

#endif // !defined(AFX_RASCLIENT_H__E30DDC47_AACE_4E00_810E_EFC69E3C91CC__INCLUDED_)
