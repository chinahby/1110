#if !defined(AFX_SKINBUTTON_H__F3A4A4C8_725E_4184_8A4D_A7A848F839FE__INCLUDED_)
#define AFX_SKINBUTTON_H__F3A4A4C8_725E_4184_8A4D_A7A848F839FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkinButton window
#include "MyBitmap.h"

class CSkinButtonResource
{

	CMyBitmap	m_bmpButton;
	int			m_TopHeight, m_LeftWidth, m_RightWidth, m_BottomHeight;
	BOOL		m_bTile;
	BOOL		m_bTrans;
	BOOL		m_bInited;

public:
	CSkinButtonResource()
	{
		m_bInited = FALSE;
	}
	
	BOOL DrawAImage( CDC *pDC, CRect r, CRect sr );
	BOOL LoadSkin( const char * skinfile );
	BOOL DrawImage(CDC *pDC, CRect r, int state);

	friend class CSkinButton;
};


class CSkinButton : public CButton
{
	DECLARE_DYNAMIC(CSkinButton)	
// Construction
	CSkinButtonResource * m_res;

	BOOL	m_bMouseIn;
	BOOL	m_bDown;
	BOOL	m_bFocus;
	BOOL	m_bEnable;

	COLORREF m_backcolor;
	COLORREF m_textcolor;
public:
	CSkinButton();


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL EnableWindow( BOOL bEnable = TRUE );
	BOOL GetRegion();
	BOOL DrawImage( CMyBitmap& bmp );
	BOOL DrawText( CDC * pDC );
	BOOL SetResource( CSkinButtonResource * res )
	{
		m_res = res;
		//GetRegion();
		return res != NULL;
	}
	virtual ~CSkinButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinButton)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINBUTTON_H__F3A4A4C8_725E_4184_8A4D_A7A848F839FE__INCLUDED_)
