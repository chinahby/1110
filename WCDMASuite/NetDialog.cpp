// NetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "NetDialog.h"
#include "Util.h"
#include <fstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetDialog dialog


CNetDialog::CNetDialog(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CNetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	iSendRate = 0;
	iRecvRate = 0;
	iSendTotal = 0;
	iRecvTotal = 0;
	m_nSize = 0;
	m_iFirst = 0;
	nOldWidth = 0;
	nOldHeigth = 0;
	nColcount = 20;
	nFirst = 0;

}


void CNetDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetDialog)
	DDX_Control(pDX, IDC_TREE_NET, m_NetTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(CNetDialog)
	ON_WM_TIMER()
	ON_MESSAGE(WM_NETWORK_SHOWRATE, OnChangRate)
	ON_MESSAGE(WM_NETWORK_TIME, OnIsStartTimer)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE_NET, OnRclickTreeNet)
	ON_COMMAND(IDM_NETTREE_CLEAR,OnNetClear)
	ON_COMMAND(IDM_NETTREE_EXPAND,OnNetExpand)
	ON_COMMAND(IDM_NETTREE_COLLAPSE,OnNetCollapse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetDialog message handlers

LONG CNetDialog::OnIsStartTimer(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 1 && lParam == 1)
	{
		SetTimer(1, 1000, NULL);
	}
	else
	{
		KillTimer(1);
		ClearZero();
	}
	return 0;
}

void CNetDialog::ClearZero()
{
	CString strTime;
	strTime.Format(TEXT("%02d:%02d:%02d"),0, 0,0);
	CString strText;
	strText = LoadStringEx(IDS_LINKTIME)+strTime;
	m_NetTree.SetItemText(m_LinkTimeTree, strText);

	CString strSendPS;
	strSendPS.Format(TEXT("%.3f KB/s "),0);
	strText = LoadStringEx(IDS_SPEED)+strSendPS;
	m_NetTree.SetItemText(m_UpLoadSpeedTree,strText);

	CString strRecvPS;
	strRecvPS.Format(TEXT("%.3f KB/s "),0);
	strText = LoadStringEx(IDS_SPEED)+strRecvPS;
	m_NetTree.SetItemText(m_DownLoadSpeedTree,strText);
}

BOOL CNetDialog::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();

	/*******************************************************/// modify ui
	CRect temprect(110, 0, 431, 251);
	CWnd::SetWindowPos(NULL, 0, 0, temprect.Width(), temprect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	/*******************************************************/
	
	CRect rcTree;
	m_NetTree.GetClientRect(rcTree);
	
	CRect rc;
	rc.left = rcTree.right+8;
	rc.top = 2;
	rc.right = rc.left+550;
	rc.bottom = rcTree.bottom+30;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE ;
	m_WndChart.Create(
		NULL,
		NULL,
		dwStyle,
		rc,
		this,
		1200,
		NULL);

	InitUI();
	
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_SPLITTER1);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER1);
	m_wndSplitter1.SetRange(140, 260, -1);


	m_NetMenu.CreatePopupMenu();
	
	m_NetMenu.AppendMenu(MF_STRING,IDM_NETTREE_CLEAR,LoadStringEx(IDS_RESETSTATISTICS));
	m_NetMenu.AppendMenu(MF_STRING,IDM_NETTREE_EXPAND,LoadStringEx(IDS_EXPANDALL));	 
	m_NetMenu.AppendMenu(MF_STRING,IDM_NETTREE_COLLAPSE,LoadStringEx(IDS_COLLAPSEALL));

	if(GetClearTime().IsEmpty())
	{
		CTime time = CTime::GetCurrentTime();
		SaveClearTime(time);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNetDialog::InitUI()
{
CString strAppPath = GetAppPath();
	CString strLanguage = GetLanguage();	
		
	strAppPath += "skin\\Other\\";

	LoadImgList(&m_ImageList, strAppPath + "net_tree.bmp");

	m_NetTree.SetImageList(&m_ImageList,LVSIL_NORMAL);
	m_CurrentLinkInfoItem = m_NetTree.InsertItem(LoadStringEx(IDS_CURRENTLINKINFO),
		NET_IMAGE_CURRENTLINKINFO,NET_IMAGE_CURRENTLINKINFO);
	m_LinkTimeTree = m_NetTree.InsertItem(LoadStringEx(IDS_LINKTIME)+"00:00:00",NET_IMAGE_OTHER,NET_IMAGE_OTHER,m_CurrentLinkInfoItem);
	HTREEITEM UploadItem = m_NetTree.InsertItem(LoadStringEx(IDS_UPLOAD),
		NET_IMAGE_UPLOAD,NET_IMAGE_UPLOAD,m_CurrentLinkInfoItem);
	m_UpLoadSpeedTree = m_NetTree.InsertItem(LoadStringEx(IDS_SPEED)+"0.00KB/s",NET_IMAGE_OTHER,NET_IMAGE_OTHER,UploadItem);
	m_UpLoadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_FLUX)+"0.00KB",NET_IMAGE_OTHER,NET_IMAGE_OTHER,UploadItem);
	HTREEITEM DownLoadItem = m_NetTree.InsertItem(LoadStringEx(IDS_DOWNLOAD),
		NET_IMAGE_DOWNLOAD,NET_IMAGE_DOWNLOAD,m_CurrentLinkInfoItem);
	m_DownLoadSpeedTree = m_NetTree.InsertItem(LoadStringEx(IDS_SPEED)+"0.00KB/s",NET_IMAGE_OTHER,NET_IMAGE_OTHER,DownLoadItem);
	m_DownLoadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_FLUX)+"0.00KB",NET_IMAGE_OTHER,NET_IMAGE_OTHER,DownLoadItem);

	m_HistoryStatInfoItem = m_NetTree.InsertItem(LoadStringEx(IDS_STATISTICS),
		NET_IMAGE_HISTORY_STATINFO,NET_IMAGE_HISTORY_STATINFO);
	m_LastClearTree = m_NetTree.InsertItem(LoadStringEx(IDS_TIMELAST)+GetClearTime(),NET_IMAGE_OTHER,NET_IMAGE_OTHER,m_HistoryStatInfoItem);
	HTREEITEM CurDayItem = m_NetTree.InsertItem(LoadStringEx(IDS_DAY),
		NET_IMAGE_STAT_DAY,NET_IMAGE_STAT_DAY,m_HistoryStatInfoItem);
	m_CurrentDayUploadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_UPLOADDATA),NET_IMAGE_OTHER,NET_IMAGE_OTHER,CurDayItem);
	m_CurrentDayDownLoadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_DOWNLOADDATA),NET_IMAGE_OTHER,NET_IMAGE_OTHER,CurDayItem);
	HTREEITEM CurMonthItem = m_NetTree.InsertItem(LoadStringEx(IDS_MONTH),
		NET_IMAGE_STAT_MONTH,NET_IMAGE_STAT_MONTH,m_HistoryStatInfoItem);
	m_CurrentMonthUploadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_UPLOADDATA),NET_IMAGE_OTHER,NET_IMAGE_OTHER,CurMonthItem);
	m_CurrentMonthDownLoadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_DOWNLOADDATA),NET_IMAGE_OTHER,NET_IMAGE_OTHER,CurMonthItem);
	HTREEITEM CurYearItem = m_NetTree.InsertItem(LoadStringEx(IDS_YEAR),
		NET_IMAGE_STAT_YEAR,NET_IMAGE_STAT_YEAR,m_HistoryStatInfoItem);
	m_CurrentYearUploadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_UPLOADDATA),NET_IMAGE_OTHER,NET_IMAGE_OTHER,CurYearItem);
	m_CurrentYearDownLoadFluxTree = m_NetTree.InsertItem(LoadStringEx(IDS_DOWNLOADDATA),NET_IMAGE_OTHER,NET_IMAGE_OTHER,CurYearItem);
	
	m_NetTree.Expand(m_CurrentLinkInfoItem, TVE_EXPAND);
	m_NetTree.Expand(UploadItem, TVE_EXPAND);
	m_NetTree.Expand(DownLoadItem, TVE_EXPAND);
	m_NetTree.Expand(m_HistoryStatInfoItem, TVE_EXPAND);
	m_NetTree.Expand(CurDayItem, TVE_EXPAND);
	m_NetTree.Expand(CurMonthItem, TVE_EXPAND);
	m_NetTree.Expand(CurYearItem, TVE_EXPAND);

	DispCurrentDayStatInfo(0,0);
	DispCurrentMonthStatInfo(0,0);
	DispCurrentYearStatInfo(0,0);
}

void CNetDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CBmpBkDialog::OnTimer(nIDEvent);
}



LONG CNetDialog::OnChangRate(WPARAM wParam, LPARAM lParam)
{	
	iSendRate = (int)lParam;
	iRecvRate = (int)wParam;
	return 0;
}

CString CNetDialog::CreatRecvRateString()
{
	CString strRecvRate ;
	strRecvRate.Format(TEXT("%d"), iRecvRate);
	return strRecvRate;
}

CString CNetDialog::CreatSendRateString()
{
	CString strSendRate ;
	strSendRate.Format(TEXT("%d"), iSendRate);
	return strSendRate;
}

void CNetDialog::OnSize(UINT nType, int cx, int cy) 
{
	CBmpBkDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect rcTree;
	CRect rcGraph;
	CRect rcSplitter1;
	if(m_NetTree.GetSafeHwnd() != NULL)
	{
		m_NetTree.GetWindowRect(&rcTree);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcTree, 2 );

	
		rcTree.bottom = cy;
		m_NetTree.MoveWindow(&rcTree);
	}
	if(m_WndChart.GetSafeHwnd() != NULL)
	{
		m_WndChart.GetWindowRect(&rcGraph);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcGraph, 2 );

		rcGraph.right = cx;
		rcGraph.bottom = cy;
		m_WndChart.MoveWindow(&rcGraph);
	}
	if(m_wndSplitter1.GetSafeHwnd() != NULL)
	{
		m_wndSplitter1.GetWindowRect(&rcSplitter1);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcSplitter1, 2 );

		rcSplitter1.bottom = cy;
		m_wndSplitter1.MoveWindow(&rcSplitter1);
	}
}

void CNetDialog::DispConnectInfo(DWORD dwConnectSecond,
								  float f_dwRecvTotal, 
								  float f_dwSendTotal, 
								  float f_dwRecvPS, 
								  float f_dwSendPS)
{
	DispCurrentLinkInfo(dwConnectSecond,f_dwRecvTotal,f_dwSendTotal,f_dwRecvPS,f_dwSendPS);
	DispCurrentDayStatInfo(f_dwRecvTotal,f_dwSendTotal);
	DispCurrentMonthStatInfo(f_dwRecvTotal,f_dwSendTotal);
	DispCurrentYearStatInfo(f_dwRecvTotal,f_dwSendTotal);
	m_WndChart.SetCount(f_dwSendPS,f_dwRecvPS);
}

void CNetDialog::DispCurrentLinkInfo(DWORD dwConnectSecond,
									  float f_dwRecvTotal, 
									  float f_dwSendTotal, 
									  float f_dwRecvPS, 
									  float f_dwSendPS)
{
	CString strTime;
	strTime.Format(TEXT("%02d:%02d:%02d"),
		dwConnectSecond/3600, 
		(dwConnectSecond%3600)/60,
		(dwConnectSecond%60));
	CString strText;
	strText = LoadStringEx(IDS_LINKTIME)+strTime;
	m_NetTree.SetItemText(m_LinkTimeTree, strText);

	CString strSendPS;
	strSendPS.Format(TEXT("%.3f KB/s "),f_dwSendPS);
	strText = LoadStringEx(IDS_SPEED)+strSendPS;
	m_NetTree.SetItemText(m_UpLoadSpeedTree,strText);

	CString strRecvPS;
	strRecvPS.Format(TEXT("%.3f KB/s "),f_dwRecvPS);
	strText = LoadStringEx(IDS_SPEED)+strRecvPS;
	m_NetTree.SetItemText(m_DownLoadSpeedTree,strText);

	CString strSendTotal;
	if(f_dwSendTotal < 1024)
		strSendTotal.Format(TEXT("%.3f KB "),f_dwSendTotal);
	else if(f_dwSendTotal > 1024 && f_dwSendTotal < 1024*1024)
		strSendTotal.Format(TEXT("%.3f MB "),f_dwSendTotal/1024);
	else
		strSendTotal.Format(TEXT("%.3f GB "),f_dwSendTotal/1024/1024);

	strText = LoadStringEx(IDS_FLUX)+strSendTotal;
	m_NetTree.SetItemText(m_UpLoadFluxTree,strText);

	CString strRecvTotal;
	if(f_dwRecvTotal < 1024)
		strRecvTotal.Format(TEXT("%.3f KB "),f_dwRecvTotal);
	else if(f_dwRecvTotal > 1024 && f_dwRecvTotal < 1024*1024)
		strRecvTotal.Format(TEXT("%.3f MB "),f_dwRecvTotal/1024);
	else
		strRecvTotal.Format(TEXT("%.3f GB "),f_dwRecvTotal/1024/1024);
	strText = LoadStringEx(IDS_FLUX)+strRecvTotal;
	m_NetTree.SetItemText(m_DownLoadFluxTree,strText);
}

void CNetDialog::DispCurrentDayStatInfo(float f_dwRecvTotal, float f_dwSendTotal)
{
	CString temp;
	__int64 i64FluxUpLoad=0;
	__int64 i64FluxDownLoad=0;
	CTime time = CTime::GetCurrentTime();
	CString strText;

	m_connectRecord.ClearVector();
	m_connectRecord.GetAllRecords();
	m_connectRecord.GetStatTime(time, time, i64FluxUpLoad,i64FluxDownLoad);
	
	double dwSendTol = (double)(f_dwSendTotal+(double)i64FluxUpLoad/ 1024);
	if(dwSendTol < 1024)
		temp.Format(TEXT("%.3f KB"), (double) (f_dwSendTotal+(double)i64FluxUpLoad/ 1024));
	else if(dwSendTol > 1024 && dwSendTol < 1024*1024)
		temp.Format(TEXT("%.3f MB"), (double)((f_dwSendTotal+(double)i64FluxUpLoad/1024)/1024));
	else
		temp.Format(TEXT("%.3f GB"), dwSendTol/1024/1024);

	strText = LoadStringEx(IDS_UPLOADDATA)+temp;
	m_NetTree.SetItemText(m_CurrentDayUploadFluxTree,strText);
	double dwRecvTol = (double) (f_dwRecvTotal+(double)i64FluxDownLoad / 1024);
	if(dwRecvTol < 1024)
		temp.Format(TEXT("%.3f KB"), dwRecvTol);
	else if(dwRecvTol > 1024 && dwRecvTol < 1024*1024)
		temp.Format(TEXT("%.3f MB"), dwRecvTol/1024);
	else
		temp.Format(TEXT("%.3f GB"), dwRecvTol/1024/1024);

	strText = LoadStringEx(IDS_DOWNLOADDATA)+temp;
	m_NetTree.SetItemText(m_CurrentDayDownLoadFluxTree,strText);
}

void CNetDialog::DispCurrentMonthStatInfo(float f_dwRecvTotal, float f_dwSendTotal)
{
	CString temp;
	__int64 i64FluxUpLoad=0;
	__int64 i64FluxDownLoad=0;
	CString strText;

	CTime time = CTime::GetCurrentTime();
	CTime	strBegTime(time.GetYear(),time.GetMonth(),1,0,0,0);
	CTime   strEndTime(time.GetYear(),time.GetMonth(),31,0,0,0);

	m_connectRecord.ClearVector();
	m_connectRecord.GetAllRecords();
	m_connectRecord.GetStatTime(strBegTime, strEndTime, i64FluxUpLoad,i64FluxDownLoad);

	double dwSendTol = (double) (f_dwSendTotal+(double)i64FluxUpLoad / 1024);
	if(dwSendTol < 1024)
		temp.Format(TEXT("%.3f KB"), dwSendTol);
	else if(dwSendTol > 1024 && dwSendTol < 1024*1024)
		temp.Format(TEXT("%.3f MB"), dwSendTol/1024);
	else
		temp.Format(TEXT("%.3f GB"), dwSendTol/1024/1024);

	strText = LoadStringEx(IDS_UPLOADDATA)+temp;
	m_NetTree.SetItemText(m_CurrentMonthUploadFluxTree,strText);

	double dwRecvTol = (double) (f_dwRecvTotal+(double)i64FluxDownLoad / 1024);
	if(dwRecvTol < 1024)
		temp.Format(TEXT("%.3f KB"), dwRecvTol);
	else if(dwRecvTol > 1024 && dwRecvTol < 1024*1024)
		temp.Format(TEXT("%.3f MB"), dwRecvTol/1024);
	else
		temp.Format(TEXT("%.3f GB"), dwRecvTol/1024/1024);
	strText = LoadStringEx(IDS_DOWNLOADDATA)+temp;
	m_NetTree.SetItemText(m_CurrentMonthDownLoadFluxTree,strText);
}

void CNetDialog::DispCurrentYearStatInfo(float f_dwRecvTotal, float f_dwSendTotal)
{
	CString temp;
	__int64 i64FluxUpLoad=0;
	__int64 i64FluxDownLoad=0;
	CString strText;

	CTime time = CTime::GetCurrentTime();
	CTime	strBegTime(time.GetYear(),1,1,0,0,0);
	CTime   strEndTime(time.GetYear(),12,31,0,0,0);

	m_connectRecord.ClearVector();
	m_connectRecord.GetAllRecords();
	m_connectRecord.GetStatTime(strBegTime, strEndTime, i64FluxUpLoad,i64FluxDownLoad);

	double dwSendTol = (double) (f_dwSendTotal+(double)i64FluxUpLoad / 1024);
	if(dwSendTol < 1024)
		temp.Format(TEXT("%.3f KB"), dwSendTol);
	else if(dwSendTol > 1024 && dwSendTol < 1024*1024)
		temp.Format(TEXT("%.3f MB"), dwSendTol/1024);
	else
		temp.Format(TEXT("%.3f GB"), dwSendTol/1024/1024);

	strText = LoadStringEx(IDS_UPLOADDATA)+temp;
	m_NetTree.SetItemText(m_CurrentYearUploadFluxTree,strText);

	double dwRecvTol = (double) (f_dwRecvTotal+(double)i64FluxDownLoad / 1024);
	if(dwRecvTol < 1024)
		temp.Format(TEXT("%.3f KB"), dwRecvTol);
	else if(dwRecvTol > 1024 && dwRecvTol < 1024*1024)
		temp.Format(TEXT("%.3f MB"), dwRecvTol/1024);
	else
		temp.Format(TEXT("%.3f GB"), dwRecvTol/1024/1024);
	
	strText = LoadStringEx(IDS_DOWNLOADDATA)+temp;
	m_NetTree.SetItemText(m_CurrentYearDownLoadFluxTree,strText);
}

LRESULT CNetDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_SPLITTER1)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize1(pHdr->delta);
		}
	}
	return CBmpBkDialog::DefWindowProc(message, wParam, lParam);
}

void CNetDialog::DoResize1(int delta)
{
	CSplitterControl::ChangeWidth(&m_NetTree, delta);
	CSplitterControl::ChangeWidth(&m_WndChart, -delta, CW_RIGHTALIGN);

	Invalidate();
	UpdateWindow();
}

void CNetDialog::OnRclickTreeNet(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
/*	CMenu NetMenu,*pPopMenu = NULL;
	NetMenu.LoadMenu(IDR_MENU_NETTREE);			
	pPopMenu = NetMenu.GetSubMenu(1);
	if(pPopMenu != NULL)
	{
		pPopMenu->ModifyMenu(IDM_NETTREE_CLEAR, MF_BYCOMMAND|MF_STRING, IDM_NETTREE_CLEAR , LoadStringEx(IDS_RESETSTATISTICS));
		pPopMenu->ModifyMenu(IDM_NETTREE_CLEAR, MF_BYCOMMAND|MF_STRING, IDM_NETTREE_CLEAR , LoadStringEx(IDS_EXPANDALL));
		pPopMenu->ModifyMenu(IDM_NETTREE_CLEAR, MF_BYCOMMAND|MF_STRING, IDM_NETTREE_CLEAR , LoadStringEx(IDS_COLLAPSEALL));
	}
*/
	CPoint pt ;	
	GetCursorPos(&pt);

	m_NetMenu.TrackPopupMenu(TPM_LEFTBUTTON,pt.x,pt.y,this);

	*pResult = 0;
}

void CNetDialog::OnNetClear()
{
	m_connectRecord.Clear();
	CTime time = CTime::GetCurrentTime();
	SaveClearTime(time);

}
void CNetDialog::OnNetExpand()
{
	m_NetTree.Expand(m_CurrentLinkInfoItem,TVE_EXPAND);
	m_NetTree.Expand(m_HistoryStatInfoItem,TVE_EXPAND);
	
}
void CNetDialog::OnNetCollapse()
{
	m_NetTree.Expand(m_CurrentLinkInfoItem,TVE_COLLAPSE);
	m_NetTree.Expand(m_HistoryStatInfoItem,TVE_COLLAPSE);
}

void CNetDialog::SaveClearTime(CTime& time)
{
	CString strFile = GetAppPath()+"time.txt";

	std::ofstream stream;
	std::string strASCII = ToMultiBytes(LPCTSTR(strFile));
	stream.open(strASCII.c_str());
	if(!stream.is_open())
		return;
	CString strTime = time.Format("%Y-%m-%d %H:%M:%S");
	stream<<ToMultiBytes(LPCTSTR(strTime)).c_str()<<std::endl;	
	stream.close();
	m_NetTree.SetItemText(m_LastClearTree,LoadStringEx(IDS_TIMELAST)+strTime);
	DispCurrentLinkInfo(0,0,0,0,0);
	DispCurrentDayStatInfo(0,0);
	DispCurrentMonthStatInfo(0,0);
	DispCurrentYearStatInfo(0,0);
}
CString CNetDialog::GetClearTime()
{
	CString strFile = GetAppPath()+"time.txt";

	std::string strASCII = ToMultiBytes(LPCTSTR(strFile));
	std::ifstream stream(strASCII.c_str());

	char buf[1024];
	stream.getline(buf,sizeof(buf)-1);

	CString strTime = buf;
	return strTime;
}
