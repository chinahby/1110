#ifndef AFX_CDMATIME_H__7C23334C_40F1_4E40_B749_46A892323727__INCLUDED_
#define AFX_CDMATIME_H__7C23334C_40F1_4E40_B749_46A892323727__INCLUDED_

class CCDMATime  
{
	CCDMATime();
public:
	static CCDMATime& Instance(); //Singleton
	~CCDMATime();

	CTime	GetCDMATime();
	void	SetCDMATime(CTime tm);
	bool	HasInited();
private:
	CCDMATime(const CCDMATime&);
	CCDMATime& operator=(const CCDMATime&);
private:
	CTime m_tmStartup;		//开始配置的时刻
	DWORD m_dwTickcount;	//开始配置时刻的系统嘀嗒

	bool  m_bConfigured;	//已经经过初始化配置
};

#endif // !defined(AFX_CDMATIME_H__7C23334C_40F1_4E40_B749_46A892323727__INCLUDED_)
