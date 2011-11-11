// MyProgressBar.cpp: implementation of the CMyProgressBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Execute.h"
#include "MyProgressBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CMyProgressBar, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyProgressBar::CMyProgressBar()
:m_OldBmp(NULL),
 m_nOffset(0)
{

}

CMyProgressBar::~CMyProgressBar()
{
	m_memDC.SelectObject(m_OldBmp);
	m_memDC.DeleteDC();
	m_bmp.DeleteObject();
}

struct TColor
{
	COLOR16 R;
	COLOR16 G;
	COLOR16 B;
};

enum{ COLOR_NUM =4};


void CMyProgressBar::BuildBMP(CDC& dc)
{
	const TColor color[COLOR_NUM+1] = 
	{
		{0xaf00,0x8500,0x5500},
		{0x5500,0xaf00,0x5500},
		{0x5f00,0x5500,0xaf00},
		{0xaf00,0xaf00,0xf500},

		{0xaf00,0x8500,0x5500},//注意和第一个点的颜色一致，以保持循环的效果
	};	

	CRect rc;
	GetClientRect(rc);
	int xOffset = rc.Width()/COLOR_NUM;

	TRIVERTEX        vert[COLOR_NUM+1] ;
	for(int i=0;i<COLOR_NUM+1;++i)
	{
		vert [i] .x      = xOffset*i;
		vert [i] .y      = (i%2)? rc.Height() : 0;
		vert [i] .Red    = color[i].R;
		vert [i] .Green  = color[i].G;
		vert [i] .Blue   = color[i].B;
		vert [i] .Alpha  = 0x0000;
	}

	GRADIENT_RECT    gRect[COLOR_NUM];
	for(i=0;i<COLOR_NUM;++i)
	{
		gRect[i].UpperLeft  = i+1;
		gRect[i].LowerRight = i;
	}

	m_memDC.CreateCompatibleDC(&dc);
	m_bmp.CreateCompatibleBitmap(&dc,rc.Width(),rc.Height());
	m_OldBmp = m_memDC.SelectObject(&m_bmp);

	GradientFill(m_memDC.m_hDC,vert,COLOR_NUM+1,gRect,COLOR_NUM,GRADIENT_FILL_RECT_H);

}

void CMyProgressBar::Paint(CDC& dc)
{
	CRect rc;
	GetClientRect(rc);
	int xOffset = rc.Width()/COLOR_NUM;
	if(m_nOffset>rc.Width())m_nOffset = m_nOffset%xOffset;

	dc.BitBlt(m_nOffset,rc.top,rc.Width()-m_nOffset, rc.Height(), &m_memDC, 0,0, SRCCOPY);
	dc.BitBlt(0,rc.top, m_nOffset, rc.Height(), &m_memDC, rc.Width()-m_nOffset,0, SRCCOPY);
}

void CMyProgressBar::OnPaint() 
{
	CPaintDC dc(this);
	static bool bFirst = true;
	if(bFirst)
	{
		BuildBMP(dc);
		bFirst = false;
	}
	Paint(dc);
}

void CMyProgressBar::OnTimer(UINT nIDEvent) 
{
	m_nOffset += 5;

	Invalidate();
}

void CMyProgressBar::StartPlay()
{
	SetTimer(1001,50, NULL);
}

void CMyProgressBar::StopPlay()
{
	KillTimer(1001);
}


