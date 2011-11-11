// EnhancedListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EnhancedListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnhancedListCtrl

bool  CEnhancedListCtrl::m_bShiftDown = false ;
bool  CEnhancedListCtrl::m_bCtrlDown  = false ;
bool  CEnhancedListCtrl::m_bAltDown	  = false ;

CEnhancedListCtrl::CEnhancedListCtrl()
{
	m_nLastItem = -1 ;
	m_nToSelCount = -1 ;
	m_bShiftDown= false ;
	m_bCtrlDown = false ;
	m_bSubItemCursor = false ;
	m_bHotAllItem = true ;
	m_nHighlight  = 0  ;
	m_nHotItem	  = -1 ;
	m_nMarkItem   = -1 ;
	m_nLSelItem   = -1 ;
	m_bReDoKeyDown=false ;
	m_bEntryLaastCol = false ;
	
	m_clrNorBK  =  RGB(0xFF,0xFF,0xFF) ;
	m_clrNorTxt =  RGB(0x00,0x00,0x00) ;
	
	m_clrMarkBK  = RGB(0xFF,0xFF,0xFF) ;
	m_clrMarkTxt = RGB(0x00,0xEF,0x00) ;
	EnableDbItemColor(false);
	m_pItemHeight = NULL ;
#if 1	
	m_clrHotBK  = RGB(0xFF,0xFF,0xFF) ;
	m_clrHotTxt = RGB(0x10,0x10,0xFF) ;

	m_clrFocusBK  = RGB(0xF8,0xB9,0x8C) ;
	m_clrFocusTxt = RGB(0x00,0x00,0x00) ;

	m_clrLFocusBK  = RGB(0xCB,0xCE,0xD1) ;
	m_clrLFocusTxt = RGB(0x00,0x00,0x00) ;
#else
	m_clrHotBK  = RGB(196,207,224) ;
	m_clrHotTxt = RGB(0,20,235) ;

	m_clrFocusBK  = RGB(100,190,255) ;
	m_clrFocusTxt = RGB(0x00,0xFF,0x00) ;

	m_clrLFocusBK  = RGB(100,190,255) ;
	m_clrLFocusTxt = RGB(0x00,0xEB,0x00) ;
#endif

}

CEnhancedListCtrl::~CEnhancedListCtrl()
{
}


BEGIN_MESSAGE_MAP(CEnhancedListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CEnhancedListCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(HDN_DIVIDERDBLCLICK, OnDividerdblclick)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_HEADERCLICKMSG, OnHeaderClickMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnhancedListCtrl message handlers

void CEnhancedListCtrl::SetHotAllItem(bool bHotItem)
{
	m_bHotAllItem = bHotItem ;
}
void CEnhancedListCtrl::SetReDoKeyDown(bool bDoFlag)
{
	m_bReDoKeyDown = bDoFlag ;
}

#ifndef  IDC_HAND 
#define  IDC_HAND MAKEINTRESOURCE(32649)
#endif

static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING ;
void CEnhancedListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	static CString strItemTxt ;
	static HCURSOR hHand    = ::LoadCursor(NULL,IDC_HAND) ;
	static HCURSOR hUpArrow = ::LoadCursor(NULL,IDC_UPARROW) ;
	HCURSOR hFlagCursor = hHand ;
	CPoint pt = point ;
	int nItem = HitTest(pt) ;
	bool bHotFlag = false ;
	m_nHotItem = -1 ;
	if(nItem != -1)
	{
		if(m_bHotAllItem )
		{
			bHotFlag = true ;
		}
		else
		{
			strItemTxt = GetItemText(nItem,0);
			if(nItem%2 == 1 && !strItemTxt.IsEmpty())
				bHotFlag = true ;
		}
		if(bHotFlag)
		{
			if(m_bSubItemCursor)
			{
				strItemTxt = GetItemText(nItem,1);
				if(!strItemTxt.IsEmpty())
				{
					LVHITTESTINFO subHit ;
					memset(&subHit,0,sizeof(LVHITTESTINFO)) ;
					subHit.pt = point ;
					if(SubItemHitTest(&subHit) != -1)
					{
						if(subHit.iSubItem == 1)
							hFlagCursor = hUpArrow ;
					}
				}
			}
			SetCursor(hFlagCursor) ;
			m_nHotItem = nItem ;
			if(m_nLastItem != nItem)
			{
				RedrawItems(m_nLastItem,m_nLastItem) ;
				m_nLastItem = nItem ;
				::PostMessage(GetParent()->m_hWnd,WM_HOTTRACKLISTITEM,(WPARAM)m_hWnd,nItem) ;
				DWORD dwState = GetItemState(nItem,dwStaFlag) ;
				if(!(dwState&LVIS_SELECTED))
				{
					//TRACE("Redraw Item %d,0x%04X \n",nItem,dwState)  ;
					RedrawItems(nItem,nItem) ;
				}
			}
		}
	}
	else
	{
		if(m_nLastItem  != -1)
		{
			TRACE(TEXT("Redraw Item %d\n"),m_nLastItem)  ;
			RedrawItems(m_nLastItem,m_nLastItem) ;
			m_nHotItem = -1 ;
			m_nLastItem  = -1 ;
		}
	}
	//²¶×½Êó±êµÄÀë¿ª
	TRACKMOUSEEVENT tme;
	tme.cbSize  = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.dwHoverTime = HOVER_DEFAULT;
	tme.hwndTrack   = m_hWnd;
	::_TrackMouseEvent( &tme );	
	CListCtrl::OnMouseMove(nFlags, point);
}


void CEnhancedListCtrl::SetMarkItem(int nItem)
{
	if(m_nMarkItem != -1)
		RedrawItems(m_nMarkItem,m_nMarkItem) ;
	m_nMarkItem = nItem ;
	if(m_nMarkItem != -1)
		RedrawItems(m_nMarkItem,m_nMarkItem) ;
	
} 

int CEnhancedListCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_nHotItem = -1 ;
	RedrawItems(m_nLastItem,m_nLastItem) ;
	m_nLastItem = -1 ;
	//TRACE("Mouse Leave !!\n") ;
	return 0;
}

int CEnhancedListCtrl::SelectAllItem()
{
	int nCount = GetItemCount() ;
	for(int i=0; i<nCount; i++)
	{
		SetItemState(i,dwStaFlag,dwStaFlag);
	}
	return nCount ;
}

void CEnhancedListCtrl::SelectItem(int nItem,bool bSetVisible)
{
	SetItemState(nItem,dwStaFlag,dwStaFlag);
	m_nLSelItem = nItem ;
	if(bSetVisible)
	{
		int nCount = GetItemCount() ;
		if(nItem == 0 )
		{
			SendMessage(WM_KEYDOWN,VK_DOWN);
			SendMessage(WM_KEYDOWN,VK_UP);
		}
		else 
		{
			SendMessage(WM_KEYDOWN,VK_UP);
			SendMessage(WM_KEYDOWN,VK_DOWN);
		}
	}
}

void CEnhancedListCtrl::UnSelectItem(int nItem)
{
	SetItemState(nItem,0,dwStaFlag);
}

int CEnhancedListCtrl::UnSelectAllItem()
{
	int nCount = GetItemCount() ;
	for(int i=0; i<nCount; i++)
	{
		SetItemState(i,0,dwStaFlag);
	}
	return nCount ;
}

BOOL CEnhancedListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	WORD nKeyVal = pMsg->wParam ;
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(nKeyVal == VK_SHIFT)
			m_bShiftDown = true ;
		if(nKeyVal == VK_CONTROL)
			m_bCtrlDown = true ;
		if(m_bCtrlDown)
		{
			if(nKeyVal == 'A')
			{
				SelectAllItem() ;
				return true ;
			}
		}
		break;
	case WM_KEYUP:
		if(nKeyVal == VK_SHIFT)
			m_bShiftDown = false ;
		if(nKeyVal == VK_CONTROL)
			m_bCtrlDown = false ;
		
		break;
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}


void CEnhancedListCtrl::SetSubitemCursor(bool bSetFlag)
{
	m_bSubItemCursor = bSetFlag;
}

void CEnhancedListCtrl::PreSubclassWindow() 
{
	ModifyStyle(0,LVS_OWNERDRAWFIXED );
	SetBkColor(RGB(255,255,255)) ;
	CHeaderCtrl *pHead = GetHeaderCtrl() ;
	if(pHead != NULL )
	m_enHeader.SubclassWindow(pHead->m_hWnd);
	DWORD dwExStyle = GetExtendedStyle() ;
	dwExStyle|=LVS_EX_FULLROWSELECT| LVS_EX_GRIDLINES /**/;
	SetExtendedStyle( dwExStyle ) ;
	CListCtrl::PreSubclassWindow();
}

void CEnhancedListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList=NULL;

	int nSavedDC = pDC->SaveDC();

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		
	GetItem(&lvi);
	//TRACE("Draw %d\n",nItem) ;

	// Should the item be highlighted
	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)||((lvi.state & LVIS_SELECTED)&&((GetFocus() == this)||
		(GetStyle() & LVS_SHOWSELALWAYS))));

	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel,  LVIR_LABEL) ;
	GetItemRect(nItem, rcIcon,   LVIR_ICON)  ;
	CRect rcCol(rcBounds); 

	CString strLabel = GetItemText(nItem, 0);
	int offset = 5;

	CRect rcHighlight(rcLabel);

	COLORREF clrItemBk  = m_clrNorBK ;
	COLORREF clrItemTxt = m_clrNorTxt;
	GetItemColor(&clrItemBk,&clrItemTxt,nItem,bHighlight?true:false);
	if(!m_bHotAllItem && nItem%2==0 )
	{
		clrItemBk  = m_clrNorBK ;
		clrItemTxt = m_clrNorTxt;
	}

	pDC->SetTextColor(clrItemTxt);
	CBrush br(clrItemBk) ;
	pDC->FillRect(rcHighlight, &br);

	// Set clip region
	rcCol.right = rcCol.left + GetColumnWidth(0);
	
	if(lvi.iImage == -1)
		pDC->FillRect(rcIcon, &br);
	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		pImageList->Draw(pDC, lvi.iImage,CPoint(rcIcon.left, rcIcon.top), ILD_TRANSPARENT );
	}
	if(GetExtendedStyle()&LVS_EX_CHECKBOXES)
	{
		CRect rcSub;
		GetSubItemRect(nItem,0,LVIR_BOUNDS,rcSub);
		rcSub.left+=4 ;
		rcSub.right = rcSub.left+rcSub.Height() ;
		static CPen pen(PS_SOLID,2,RGB(0,0,0));
		pDC->SelectObject(&pen) ;
		if(rcSub.Width()>18)
			rcSub.InflateRect(-2,-2,-2,-2) ;
		rcSub.InflateRect(-2,-2,-3,-3) ;
		pDC->Rectangle(&rcSub) ;
		if(GetCheck(nItem))
		{
			static int nArry[]={5,6,7,6,5,4,3};
			static CPen Fpen(PS_SOLID,1,RGB(0,0,0));
			pDC->SelectObject(&Fpen) ;
			for(int i=0; i<7;i++)
			{
				int nx = rcSub.left+2+i;
				int ny = rcSub.top+nArry[i]-1;
				pDC->MoveTo(nx,ny)   ;
				pDC->LineTo(nx,ny+3) ;
			}
		}
	}
	
	DWORD dwTxtFormat = DT_SINGLELINE |DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS;

	// Draw item label - Column 0
	rcLabel.left  += offset/2;
	rcLabel.right -= offset;
	pDC->DrawText(strLabel,-1,rcLabel,dwTxtFormat);

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right:rcBounds.right;
	//int nColCount  = m_enHeader.GetItemCount() ;
	for(int i=1; GetColumn(i, &lvc)  ; i++)
	{
		rcCol.left   = rcCol.right;
		rcCol.right += lvc.cx;
		pDC->FillRect(rcCol, &br);
		strLabel = GetItemText(nItem,i);
		if (strLabel.IsEmpty())
			continue;
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;
		default:
			break;
		}

		rcLabel = rcCol;
		rcLabel.left  += offset;
		rcLabel.right -= offset;
		if(m_bEntryLaastCol)
		{
			dwTxtFormat &= ~DT_END_ELLIPSIS  ;
			rcLabel.right += 5 ;
			rcLabel.left -= 1 ;
		}
		if(rcLabel.Width()>10)
			pDC->DrawText(strLabel, rcLabel,  nJustify|dwTxtFormat);
	}

	br.DeleteObject();
	if (lvi.state & LVIS_FOCUSED && (GetFocus() == this))
	{
		bool bDrawFoucs = true ;
		if(!m_bHotAllItem && nItem%2==0 )
		{
			bDrawFoucs = false ;
		}
		if(bDrawFoucs)
		{
			CRect wRc(lpDrawItemStruct->rcItem);
			if(lvi.iImage != -1)
				wRc.left+=(rcIcon.Width()+offset-2) ;
			else
				wRc.left+=2 ;

			wRc.bottom -= 1 ;
			pDC->DrawFocusRect(wRc); 
		}
	}
	pDC->RestoreDC(nSavedDC);
}

void CEnhancedListCtrl::GetItemColor(COLORREF *pClrBk,COLORREF *pClrTxt,int nItem, bool bHighLight)
{
	COLORREF clrItemBk  = m_clrNorBK  ;
	COLORREF clrItemTxt = m_clrNorTxt ;
	if(nItem%2 && m_bEnableDbItemColor)
		clrItemBk  = m_clrDbItemBK  ;
	if(bHighLight)
	{
		clrItemBk  = m_clrFocusBK ;
		clrItemTxt = m_clrFocusTxt ;
		if(GetFocus() != this )
		{
			clrItemBk  = m_clrLFocusBK ;
			clrItemTxt = m_clrLFocusTxt ;
		}
	}
	else
	{
		if( m_nHotItem == nItem )
		{
			clrItemBk  = m_clrHotBK ;
			clrItemTxt = m_clrHotTxt ;
		}
	}
	if(m_nMarkItem == nItem)
	{
		clrItemBk  = m_clrMarkBK ;
		clrItemTxt = m_clrMarkTxt ;
	}

	if(clrItemBk == clrItemTxt)
	{
		clrItemTxt = 0x00FFFFFF - clrItemTxt ;
	}
	*pClrBk  = clrItemBk ;
	*pClrTxt = clrItemTxt ;
}

void CEnhancedListCtrl::EnableDbItemColor(bool bEnable,COLORREF clr)
{
	m_bEnableDbItemColor = bEnable ;
	m_clrDbItemBK = clr ;
}



void CEnhancedListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nItem = HitTest(point) ;
	if(GetExtendedStyle()&LVS_EX_CHECKBOXES)
	{
		CRect rcSub;
		GetSubItemRect(nItem,0,LVIR_BOUNDS,rcSub);
		rcSub.right = rcSub.left+20 ;
		if(rcSub.PtInRect(point))
		{
			SetCheck(nItem,!GetCheck(nItem)) ;
		}
	}
	m_nLSelItem = nItem ;
	
	CListCtrl::OnLButtonDown(nFlags, point);
}

void CEnhancedListCtrl::SetItemHeight(int nItemHeight)
{
	if(m_pItemHeight != NULL)
		delete m_pItemHeight ;
	m_pItemHeight = new CImageList() ;
	m_pItemHeight->Create(20,nItemHeight,ILC_COLOR,1,1);
	SetImageList(m_pItemHeight,LVSIL_SMALL) ;
}

void CEnhancedListCtrl::SetHeaderCtrlColor(COLORREF clrHeader,COLORREF clrLine)
{
	m_enHeader.SetHeadBkColor(clrHeader,clrLine) ;
}

void CEnhancedListCtrl::OnPaint() 
{
	try
	{
		CPaintDC dc(this); // device context for painting
 		CDC *pDC = &dc ;
		CRect    rect    ;
		GetClientRect(&rect);
		CMemDC memDC(pDC, &rect, FALSE);

		CRect hRc;
		CWnd *pWnd = GetDlgItem(0) ;
		if(pWnd != NULL)
		{
			pWnd->GetWindowRect(&hRc);
			ScreenToClient(&hRc);
			pDC->ExcludeClipRect(&hRc);
		}
		DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0);
	}
	catch(...)
	{
	}
}

BOOL CEnhancedListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	//turn CListCtrl::OnEraseBkgnd(pDC) ;
	return  FALSE ;
}


void CEnhancedListCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN *)pNMHDR;
	WORD nKey = pLVKeyDow->wVKey;
	if(nKey == VK_DELETE || nKey == 'D' || nKey == 'd')
	{
		nKey = LISTCOMMAND_DELETE ;
	}
	*pResult = 0;
	::PostMessage(GetParent()->GetSafeHwnd(),WM_LISTKEYDOWNMSG,GetDlgCtrlID() ,(LPARAM)nKey);
	int nItemCount = GetItemCount() ;
	if(nItemCount==0 )
		return ;

	int nItem = m_nLSelItem;
	switch(nKey)
	{
	case VK_LEFT:
		if(m_bCtrlDown)
			return ;
	case VK_UP  :
		nItem--;
		break ;

	case VK_RIGHT:
		if(m_bCtrlDown)
			return ;
	case VK_DOWN :
		nItem++;
		break;
	case VK_SPACE:
		{
			CRect itemRc ;
			GetItemRect(nItem,&itemRc,LVIR_BOUNDS) ;
			//ClientToScreen(itemRc) ;
			SendMessage(WM_LBUTTONDBLCLK,MK_LBUTTON,MAKELONG(itemRc.left+2,itemRc.top+2)) ;
		}
		break;
	}
	if(nItem < 0)
	{
		nItem = nItemCount-1 ;
	}
	else if(nItem >= nItemCount)
	{
		nItem = 0 ;
	}
	switch(nKey)
	{
	case VK_LEFT :
	case VK_RIGHT:
		UnSelectItem(m_nLSelItem);
		SelectItem(nItem) ;
		break ;
	case VK_UP   :
	case VK_DOWN :
		if(m_bShiftDown)
			break;
		m_nLSelItem = nItem ;
		SetTimer(2,10,NULL) ;
		break;
	}	
}

void CEnhancedListCtrl::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 2)
	{
		KillTimer(2) ;
		UnSelectAllItem();
		SelectItem(m_nLSelItem) ;
	}
	else if(nIDEvent == 3)
	{
		KillTimer( 3);
		if(m_nToSelCount>0)
		{
			int nItemCount = GetItemCount() - 1 ;
			for(int i=0; i<m_nToSelCount; i++)
			{
				 SelectItem(nItemCount-i) ;
			}
			m_nToSelCount=-1;
		}
	}
		 
	CListCtrl::OnTimer(nIDEvent);
}


void CEnhancedListCtrl::OnHeaderClickMsg(WPARAM wParam,LPARAM lParam)
{
	::PostMessage(GetParent()->GetSafeHwnd(),WM_HEADERCLICKMSG,wParam,lParam );
}

void CEnhancedListCtrl::SetItemIconIndex(int nItem,int nIndex)
{
	LVITEM Item;
	memset(&Item,0,sizeof(LVITEM)) ;
	Item.mask  = TCIF_IMAGE ;
	Item.iItem = nItem ;
	Item.iSubItem = 0 ;
	GetItem(&Item);
	Item.iImage = nIndex ;
	SetItem(&Item);
}

void CEnhancedListCtrl::SetLastColEntry(bool bEntry)
{
	m_bEntryLaastCol = bEntry ;
}

void CEnhancedListCtrl::SetLastItemsVisible(int nItemCount)
{
	m_nToSelCount = nItemCount ;
	SelectItem(GetItemCount()-1,true);
	SetTimer(3,50,NULL) ;
}

void CEnhancedListCtrl::OnDividerdblclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CEnhancedListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
