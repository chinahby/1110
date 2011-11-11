//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#if !defined(AFX_NETDIALOG_H__8B1C0558_EE2C_4C6B_B8A7_44B1F2A82383__INCLUDED_)
#define AFX_NETDIALOG_H__8B1C0558_EE2C_4C6B_B8A7_44B1F2A82383__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetDialog.h : header file
//
#include "BmpBkDialog.h"
#include "ResizeCtrl.h"
#include "CWndChart.h"
#include "Data/ConnectRecord.h"
#include "SplitterControl.h"

/////////////////////////////////////////////////////////////////////////////
// CNetDialog dialog

#define NET_IMAGE_CURRENTLINKINFO	0
#define NET_IMAGE_UPLOAD			8
#define NET_IMAGE_DOWNLOAD			7
#define NET_IMAGE_HISTORY_STATINFO		6
#define NET_IMAGE_STAT_DAY				4
#define NET_IMAGE_STAT_MONTH				3
#define NET_IMAGE_STAT_YEAR				2
#define NET_IMAGE_OTHER					1

class CNetDialog : public CBmpBkDialog
{
// Construction
public:
	CNetDialog(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CNetDialog)
	enum { IDD = IDD_DIALOG_NETWORK };
	CTreeCtrl	m_NetTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementatio
private:
	CString CreatRecvRateString();
	CString CreatSendRateString();
	CString  result[50];
	CString  sendResult[50];
	int   nColcount;
	int   iSendRate;
	int   iRecvRate;
	int   iSendTotal;
	int   iRecvTotal;
	int   m_nSize;
	int   m_iFirst;
	int   nOldWidth;
	int   nOldHeigth;
	int   nMaxRecvHeigth;
	int   nMaxSendHeigth;
	int   nFirst;
	
private:
	CWndChart m_WndChart;
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnChangRate(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnIsStartTimer(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRclickTreeNet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNetClear();
	afx_msg void OnNetExpand();
	afx_msg void OnNetCollapse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void DispConnectInfo(DWORD dwConnectSecond,float f_dwRecvTotal, float f_dwSendTotal, float f_dwRecvPS, float f_dwSendPS);
private:
	private:
	void InitUI();
	void DispCurrentLinkInfo(DWORD dwConnectSecond,float f_dwRecvTotal, float f_dwSendTotal, float f_dwRecvPS, float f_dwSendPS);
	void DispCurrentDayStatInfo(float f_dwRecvTotal, float f_dwSendTotal);
	void DispCurrentMonthStatInfo(float f_dwRecvTotal, float f_dwSendTotal);
	void DispCurrentYearStatInfo(float f_dwRecvTotal, float f_dwSendTotal);
	void DoResize1(int delta);
	void SaveClearTime(CTime& time);
	CString GetClearTime();
	void ClearZero();
public:
		CConnectRecord m_connectRecord;
private:
	CImageList m_ImageList;
	HTREEITEM m_LinkTimeTree;
	HTREEITEM m_UpLoadSpeedTree;
	HTREEITEM m_UpLoadFluxTree;
	HTREEITEM m_DownLoadSpeedTree;
	HTREEITEM m_DownLoadFluxTree;
	HTREEITEM m_CurrentDayUploadFluxTree;
	HTREEITEM m_CurrentDayDownLoadFluxTree;
	HTREEITEM m_CurrentMonthUploadFluxTree;
	HTREEITEM m_CurrentMonthDownLoadFluxTree;
	HTREEITEM m_CurrentYearUploadFluxTree;
	HTREEITEM m_CurrentYearDownLoadFluxTree;
	HTREEITEM m_LastClearTree;
	CSplitterControl m_wndSplitter1;
	CMenu m_NetMenu;
	HTREEITEM m_CurrentLinkInfoItem ;
	HTREEITEM m_HistoryStatInfoItem;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETDIALOG_H__8B1C0558_EE2C_4C6B_B8A7_44B1F2A82383__INCLUDED_)
