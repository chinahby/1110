#pragma once

#include <vector>

class CWndChart: public CWnd
{
public:
	CWndChart();
	~CWndChart();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
public:
	void SetCount(float fUpload,float fDownload );
private:
	virtual void Draw(CDC* memDC, const CRect& rc);
	void DrawSpeed(CDC* memDC, const CRect& rc);
	void DrawBox(CDC* memDC, const CRect& rc);
private:
	std::vector<float> m_fUploadCount;
	std::vector<float> m_fDownloadCount;
	int m_colNum;
};
