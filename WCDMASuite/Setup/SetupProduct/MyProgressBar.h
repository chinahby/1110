// MyProgressBar.h: interface for the CMyProgressBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYPROGRESSBAR_H__4FDC8ABE_9C89_4C60_8E4E_8B28A1D4ECA3__INCLUDED_)
#define AFX_MYPROGRESSBAR_H__4FDC8ABE_9C89_4C60_8E4E_8B28A1D4ECA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyProgressBar : public CWnd  
{
public:
	CMyProgressBar();
	virtual ~CMyProgressBar();

	void StartPlay();
	void StopPlay();
protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
private:
	void BuildBMP(CDC& dc);
	void Paint(CDC& dc);
private:
	CDC m_memDC;
	CBitmap* m_OldBmp;
	CBitmap  m_bmp;

	int m_nOffset;
};

#endif // !defined(AFX_MYPROGRESSBAR_H__4FDC8ABE_9C89_4C60_8E4E_8B28A1D4ECA3__INCLUDED_)
