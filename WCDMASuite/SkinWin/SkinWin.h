// SkinWin.h: interface for the CSkinWin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINWIN_H__F010D322_E34F_4F9E_87E8_E76E9611942D__INCLUDED_)
#define AFX_SKINWIN_H__F010D322_E34F_4F9E_87E8_E76E9611942D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyBitmap.h"
#include "Subclass.h"
class CSkinWin  : public CSubclassWnd
{
public:
	CString		m_title;
	CString		m_strSkinFile;		//skin definition file
	CMyBitmap	m_bmpTitle;		//contains two bitmap;

	CMyBitmap	m_bmpLeft;		
	CMyBitmap	m_bmpRight;
	CMyBitmap	m_bmpBottom;
	CMyBitmap	m_bmpMenubg;
	CMyBitmap	m_bmpDlg;

	UINT		m_oldHitTest;
	UINT		m_moveHitTest;
	UINT		m_downHitTest;
	//offset in the bmp
	int			m_titleoff1, m_titleoff2;
	int			m_leftoff1, m_leftoff2;
	int			m_rightoff1, m_rightoff2;
	int			m_bottomoff1, m_bottomoff2;

	CMyBitmap	m_bmpMinBtn;
	CMyBitmap	m_bmpMaxBtn;
	CMyBitmap	m_bmpCloseBtn;
	CMyBitmap	m_bmpRestoreBtn;
	CRect		m_rectMinBtn;
	CRect		m_rectMaxBtn;
	CRect		m_rectCloseBtn;
	CRect		m_rectRestoreBtn;

	CRect		m_rectRestoreWin;
	int			m_winstate;			//0 for normal, 1 for max, 2 for min
	int			m_oldwinstate;		//save state before min



	BOOL		m_sizable;
	BOOL		m_minable;
	BOOL		m_maxable;
	BOOL		m_sysmenu;

	int			m_TitleHeight;
	int			m_BorderLeftWidth;
	int			m_BorderRightWidth;
	int			m_BorderBottomHeight;

	int			m_textShift, m_textShiftVer;
	COLORREF	m_colTitle1, m_colTitle2;
	COLORREF	m_colTrans;

	COLORREF	m_barcolor;
	COLORREF	m_menucolor;
	COLORREF	m_btnbgcolor, m_btntextcolor, m_btnhovercolor, m_btnfocuscolor;

	BOOL		m_bTrans;

	BOOL		m_bInit;
	BOOL		m_bActive;
public:
	BOOL IsInited()
	{
		return m_bInit;
	}
	BOOL HandleSysCommand(WPARAM wp, LPARAM lp);
	BOOL PopupSysMenu(CPoint point);
	void OnSysCommand(UINT nID, LPARAM lParam );
	BOOL Restore();
	BOOL Minimize();
	BOOL Maximize();
	LRESULT OnSetText( WPARAM wp, LPARAM lp );
	void OnActivate(UINT nState,CWnd* pWndOther, BOOL bMinimized );
	CRect GetMaximizeRect();
	BOOL OnEraseBkgnd( CDC *pDC);
	void OnSizing( UINT nSide, LPRECT lpRect);
	void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	void OnGetMinMaxInfo( MINMAXINFO* lpMMI );
	BOOL DrawButton( CDC * pDC, int i, int state );
	CRect GetButtonRect( int i );
	void OnNcRButtonUp(   UINT nHitTest,   CPoint point);
	void OnNcRButtonDown(   UINT nHitTest, CPoint point );
	void OnNcMouseMove( UINT nHitTest, CPoint point );
	void OnNcLButtonUp( UINT nHitTest, CPoint point );
	void OnNcLButtonDown( UINT nHitTest, CPoint point );
	void OnNcLButtonDblClk( UINT nHitTest,  CPoint point );
	UINT OnNcHitTest( CPoint point );
	BOOL OnNcActivate( BOOL bActive );
	HRGN GetRegion( int w, int h);
	void OnSize(UINT nType, int cx, int cy );
	void OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	void OnNcPaint( HRGN rgn );
	BOOL InstallSkin( CWnd * wnd );
	BOOL DrawFrame( CDC *pDC, int x, int y,int w, int h, int state, int title = 1 );
	BOOL DrawRight(CDC *pDC, int x, int y, int h, int state);
	BOOL DrawLeft(CDC *pDC, int x, int y, int h, int state);
	BOOL DrawBottom(CDC *pDC, int x, int y, int w, int state);
	BOOL DrawTitle( CDC *pDC, int x, int y, int w, int state );
	CSkinWin();
	CSkinWin( const char * skinfile )
	{
		CSkinWin();
		LoadSkin( skinfile );			
	}
	virtual ~CSkinWin();

	//skinfile:	full path name to skin inifile
	BOOL	LoadSkin( const char * skinfile );
	//vitrual functions
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);
	void PaintRound(CDC* pDC);

};

#endif // !defined(AFX_SKINWIN_H__F010D322_E34F_4F9E_87E8_E76E9611942D__INCLUDED_)
