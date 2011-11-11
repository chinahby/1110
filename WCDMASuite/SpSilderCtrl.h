#if !defined(AFX_SPSILDERCTRL_H__AEA30DFE_5312_44C2_AB19_F6E0588C72F2__INCLUDED_)
#define AFX_SPSILDERCTRL_H__AEA30DFE_5312_44C2_AB19_F6E0588C72F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpSilderCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpSilderCtrl window

class CSpSilderCtrl : public CStatic
{
// Construction
public:
	CSpSilderCtrl();
	CRect  m_rcThumb  ;
	CRect  m_rcBack   ;
	bool   m_bDraging ;
	DWORD  m_dwMinPos ;
	DWORD  m_dwMaxPos ;
	DWORD  m_dwCurPos ;
	int    m_nClickPt ;
	int	   m_nThumbWidth;
	int	   m_nThumbHeight;
	int	   m_nBackWidth;
	int	   m_nBackHeight;
	int	   m_nBackMapWidth;
	int    m_nHMargin ;
	HWND   m_hParentWnd ;
	CBitmap *m_pBkBitMap ;
	CBitmap *m_pThBitMap ;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpSilderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	bool GetDraging();
	void SetMargin(int nMargin);
	void DrawTransparentBitmap(CDC *pDC,CBitmap *pTest,CRect *pBmpRc,COLORREF crColour) ;
	void DrawTransparentBitmap(CDC *pDC, UINT IDImage,CRect *pBmpRc, COLORREF rgbMask);
	void SetSLBitmap(int nBkMapID,int nThumbMapID);
	void SendPosChangeMessage();
	DWORD GetSLPos();
	void SetSLPos(DWORD dwPos);
	void SetSLRange(DWORD dwBegin,DWORD dwEnd);
	void ResetThumbRect(const CPoint *pNewPt);
	void InitSlider();
	void DrawSlider();
	virtual ~CSpSilderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpSilderCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPSILDERCTRL_H__AEA30DFE_5312_44C2_AB19_F6E0588C72F2__INCLUDED_)
