#include "StdAfx.h"
#include "CWndChart.h"
#include "Util.h"
#include "Resource.h"
#include "GlobalDataCenter.h"

BEGIN_MESSAGE_MAP(CWndChart, CWnd)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CWndChart::CWndChart()
{
}

CWndChart::~CWndChart()
{
}

BOOL CWndChart::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	CBitmap bmpTemp;
	CDC memDC;

	GetClientRect(rc);
	memDC.CreateCompatibleDC(pDC);
	bmpTemp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());


	CBitmap* oldBMP = memDC.SelectObject(&bmpTemp);
	
	Draw(&memDC, rc);

	pDC->BitBlt(0,0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(oldBMP);
	oldBMP->DeleteObject();
	memDC.DeleteDC();
	ReleaseDC(&memDC);
	bmpTemp.DeleteObject();

	return false;
}

void CWndChart::DrawSpeed(CDC* memDC, const CRect& rc)
{
	CFont newFont;
	newFont.CreateFont(
		12,                        // nHeight
		0,                         // nWidth
		900,                         // nEscapement
		900,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		TEXT("Arial"));                 // lpszFacename

	CFont* oldFont = memDC->SelectObject(&newFont);

	memDC->TextOut(rc.left+5,rc.top+rc.Height()/2-35,LoadStringEx(IDS_PAINTSPEED));
	memDC->SelectObject(oldFont);
	newFont.DeleteObject();
}

void CWndChart::DrawBox(CDC* memDC, const CRect& rc)
{
	int scale = rc.Height()/13;
	CBrush brushOr(RGB(162,162,0));
	int ny = scale*12+5;
	CBrush* oldBrush = memDC->SelectObject(&brushOr);
	memDC->Rectangle(55,ny+2,65,ny+12);

	brushOr.DeleteObject();
	
	int x = 0;
	if(GetLanguage() == "-Ch")
		x = 120;
	else
		x = 150;
	
	CBrush brushGr(RGB(15,144,15));
	memDC->SelectObject(&brushGr);
	memDC->Rectangle(x,ny+2,x+10,ny+12);
	memDC->SelectObject(oldBrush);
	brushGr.DeleteObject();

	int nHeight =0;
	if(GetLanguage() == "-Ch")
		nHeight = 15;
	else
		nHeight = 14;
	CFont newFont;
	newFont.CreateFont(
		nHeight,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		TEXT("Arial"));                 // lpszFacename

	CFont* oldFont = memDC->SelectObject(&newFont);
	CString str = LoadStringEx(IDS_UPLOADSPEED);
	
	if(GetLanguage() == "-Ch")
		x = 90;
	else
		x = 100;
	memDC->TextOut(x,ny,str);

	if(GetLanguage() == "-Ch")
		x = 155;
	else
		x = 205;
	str = LoadStringEx(IDS_DOWNLOADSPEED);
	memDC->TextOut(x,ny,str);

	memDC->SelectObject(oldFont);
	newFont.DeleteObject();
}

void CWndChart::Draw(CDC* memDC, const CRect& rc)
{
	memDC->FillSolidRect(rc, RGB(0, 0, 0));

//画网格
	CPen gridPen(PS_SOLID, 1, RGB(0, 234, 234));
	CPen* oldGrid = memDC->SelectObject(&gridPen);
	int scale = rc.Height()/13;
	int colNum =  rc.Width()/scale;    //根据高度计算刻度
	int rowNum = rc.Height()/scale;
	
	m_colNum = (rc.Width()-50)/5;
	
	memDC->MoveTo(50,5);
	memDC->LineTo(50,scale*12);

	memDC->MoveTo(rc.right-5,5);
	memDC->LineTo(rc.right-5,scale*12);


	memDC->MoveTo(50,5);
	memDC->LineTo(rc.right-5,5);

	memDC->MoveTo(50,scale*12);
	memDC->LineTo(rc.right-5,scale*12);
	//memDC->SelectObject(oldGrid);

	CPen DotPen(PS_DOT, 1, RGB(0, 234, 234));
	memDC->SelectObject(&DotPen);

	for(int i=1; i<=11; ++i)
	{	
		memDC->MoveTo(50,5+scale*i);
		memDC->LineTo(rc.right-5,5+scale*i);	
	}

	memDC->SetTextColor(RGB(0,234,234));
	memDC->SetTextAlign(TA_CENTER);
	
	int textHeigth = 15;
	CFont newFont;
	newFont.CreateFont(
					   textHeigth,                        // nHeight
					   0,                         // nWidth
					   0,                         // nEscapement
					   0,                         // nOrientation
					   FW_NORMAL,                 // nWeight
					   FALSE,                     // bItalic
					   FALSE,                     // bUnderline
					   0,                         // cStrikeOut
					   ANSI_CHARSET,              // nCharSet
					   OUT_DEFAULT_PRECIS,        // nOutPrecision
					   CLIP_DEFAULT_PRECIS,       // nClipPrecision
					   DEFAULT_QUALITY,           // nQuality
					   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
					   TEXT("Arial"));                 // lpszFacename

	CFont* oldFont = memDC->SelectObject(&newFont);
	
	
	CClientDC dc(this);	
	int nHorz = dc.GetDeviceCaps(HORZRES);	
	int nVert = dc.GetDeviceCaps(VERTRES);
	int xy = 2;	
	memDC->TextOut(31, 0, TEXT("240.0"));
	memDC->TextOut(31, scale- xy*(scale/10), TEXT("220.0"));
	memDC->TextOut(31, scale*2 - xy*(scale/10), TEXT("200.0"));
	memDC->TextOut(31, scale*3 - xy*(scale/10), TEXT("180.0"));
	memDC->TextOut(31, scale*4 - xy*(scale/10),TEXT("160.0"));
	memDC->TextOut(31, scale*5 - xy*(scale/10), TEXT("140.0"));
	memDC->TextOut(31, scale*6 - xy*(scale/10), TEXT("120.0"));
	memDC->TextOut(31, scale*7 - xy*(scale/10), TEXT("100.0"));
	memDC->TextOut(35, scale*8 - xy*(scale/10), TEXT("80.0"));
	memDC->TextOut(35, scale*9 - xy*(scale/10), TEXT("60.0"));
	memDC->TextOut(35, scale*10 - xy*(scale/10), TEXT("40.0"));
	memDC->TextOut(35, scale*11 - xy*(scale/10), TEXT("20.0"));
	memDC->TextOut(39, scale*12 - xy*(scale/10), TEXT("0.0"));


	
	memDC->SelectObject(oldFont);
	memDC->SelectObject(oldGrid);
	oldFont->DeleteObject();
	//oldGrid->DeleteObject();
	
//画流量图
	CPen pen(PS_SOLID, 2, RGB(162,162,0));
	CPen* oldPen = memDC->SelectObject(&pen);	
	if(m_fUploadCount.size())
	{
		memDC->MoveTo(51, scale*12 - ((float)m_fUploadCount[0]*(12*scale)/240.0));
		int nSize = m_fUploadCount.size();
		//if(nSize>m_colNum-1)
		//	m_fUploadCount.erase(m_fUploadCount.begin(),m_fUploadCount.begin()+nSize-m_colNum+1);
		//else
		//	nSize = m_fUploadCount.size();
		bool berase = false;
		for(size_t i=1; i<m_fUploadCount.size(); ++i)
		{			
			if(51+5*i>rc.right-5)
			{
				berase = true;
				memDC->LineTo(rc.right-5, 12*scale - ((float)m_fUploadCount[i]*(12*scale)/240.0));
				break;
			}
			else
				memDC->LineTo(51+5*i, 12*scale - ((float)m_fUploadCount[i]*(12*scale)/240.0));
		}
		if(berase)
			m_fUploadCount.erase(m_fUploadCount.begin(),m_fUploadCount.begin()+nSize-m_colNum+1);
	}	

	CPen penDownload(PS_SOLID, 2, RGB(15,144,15));
	memDC->SelectObject(&penDownload);	
	if(m_fDownloadCount.size())
	{
		memDC->MoveTo(51, scale*12 - ((float)m_fDownloadCount[0]*(12*scale)/240.0));
		int nSize = m_fDownloadCount.size();
		//if(nSize>m_colNum-1)
		//	m_fDownloadCount.erase(m_fDownloadCount.begin(),m_fDownloadCount.begin()+nSize-m_colNum+1);
		//else
		//	nSize = m_fDownloadCount.size();
		bool berase = false;
		for(size_t i=1; i<m_fDownloadCount.size(); ++i)
		{			
			if(51+5*i>rc.right-5)
			{
				berase = true;
				memDC->LineTo(rc.right-5, 12*scale - ((float)m_fDownloadCount[i]*(12*scale)/240.0));				
			}
			else
				memDC->LineTo(51+5*i, 12*scale - ((float)m_fDownloadCount[i]*(12*scale)/240.0));
			if(berase)
				m_fDownloadCount.erase(m_fDownloadCount.begin(),m_fDownloadCount.begin()+nSize-m_colNum+1);
		}
	}

	pen.DeleteObject();
	newFont.DeleteObject();
	gridPen.DeleteObject();
	DotPen.DeleteObject();
	//oldPen->DeleteObject();	
	memDC->SelectObject(oldGrid);

	DrawSpeed(memDC,rc);
	DrawBox(memDC,rc);
}

void CWndChart::SetCount(float fUpload,float fDownload)
{
	if((fUpload <0)||(fDownload <0))
		return;
	if((fUpload > 240.0)||(fDownload > 240.0))
		return;
	m_fUploadCount.push_back(fUpload);
	if(m_fUploadCount.size()>m_colNum)
		m_fUploadCount.erase(m_fUploadCount.begin());

	m_fDownloadCount.push_back(fDownload);
	if(m_fDownloadCount.size()>m_colNum)
		m_fDownloadCount.erase(m_fDownloadCount.begin());

	Invalidate();
}
