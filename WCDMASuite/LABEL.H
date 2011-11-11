#if !defined(AFX_LABEL_H__59128F70_6CC6_11D4_BD73_000021479D21__INCLUDED_)
#define AFX_LABEL_H__59128F70_6CC6_11D4_BD73_000021479D21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Label.h : header file
//

#define	NM_LINKCLICK	    (WM_APP + 0x200)
#define	WM_CLICKSTATICCTRL	(WM_USER + 0x300)

/////////////////////////////////////////////////////////////////////////////
// CLabel window

class CLabel : public CStatic
{
// Construction
public:
	CLabel();
	virtual ~CLabel();
	void SetFlaseColor(COLORREF crText,COLORREF crBkgnd=RGB(0,0,0));
	void SetBkColor(COLORREF crBkgnd);
	void SetTextColor(COLORREF crText);
	void SetLabelText(LPCSTR lpLabelTxt=NULL);
	void SetFontBold(bool bBold=true);
	void SetFontName(LPCTSTR lpFontName=NULL);
	void SetFontUnderline(bool bSet=true);
	void SetFontItalic(bool bSet=true);
	void SetFontSize(int nSize);

	void SetLink(bool bLink,bool bNotifyParent);
	void SetLinkCursor(HCURSOR hCursor);
	void SetRotationAngle(UINT nAngle,bool bRotation=true);
	void DrawLabel(CDC *pDC) ;
// Attributes
protected:
	void ReconstructFont();
	COLORREF	m_crBkGround;
	COLORREF	m_crFlashBkGround;
	COLORREF	m_crText;
	COLORREF	m_crFlashText;
	LOGFONT		m_lf;
	CFont		m_font;
	CString		m_strText;
	bool		m_bState;
	bool		m_bTimer;
	bool		m_bLink;
	bool		m_bTransparent;
	bool		m_bNotifyParent;
	bool		m_bRotation;
	COLORREF	m_crCheck;
	HCURSOR		m_hCursor;
	bool		m_bSetCircle ;
	bool		m_bSetCheck ;
	bool		m_bLAlign  ;
	bool		m_bitmapBk ;
	DWORD		m_bitmapID ;
	CDC         *m_pParentBKMemDC;
	CBitmap		*m_pBkbitmap;
	int			m_nParXPos ;
	int			m_nParYPos ;
	int			m_nParWidth  ;
	int			m_nParHeight ;
	bool		m_bSetHand ;
	// Operations
public:
	void SetParentBkMemDC(CDC *pPBkMemDC,int nX,int nY,int nWidth,int nHeight);
	void SetBkBitmap(DWORD dwbtID);
	void SetBkBitmap(CBitmap *pBkbitmap=NULL);
	void SetLeftAlign(bool bLAlign=true);
	void SetHandCursor(bool bHand=true);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabel)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetFlashTime(DWORD dwTime);
	void SetTransparent(bool bTrans=true);

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabel)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
	virtual	void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABEL_H__59128F70_6CC6_11D4_BD73_000021479D21__INCLUDED_)
