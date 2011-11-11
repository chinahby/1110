#include "stdafx.h"
#include "SortListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LPCTSTR g_pszSection = _T("ListCtrls");

static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING ;

struct ItemData
{
public:
	ItemData() : arrpsz( NULL ), dwData( NULL ) {}

	LPTSTR* arrpsz;
	DWORD dwData;

private:
	// ban copying.
	ItemData( const ItemData& );
	ItemData& operator=( const ItemData& );
};

CSortListCtrl::CSortListCtrl()
	: m_iNumColumns( 0 )
	, m_iSortColumn( -1 )
	, m_bSortAscending( TRUE )
{
	
}


CSortListCtrl::~CSortListCtrl()
{

}


BEGIN_MESSAGE_MAP(CSortListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSortListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSortListCtrl message handlers

void CSortListCtrl::PreSubclassWindow()
{
	// the list control must have the report style.
	ASSERT( GetStyle() & LVS_REPORT );

	CListCtrl::PreSubclassWindow();
	VERIFY( m_ctlHeader.SubclassWindow( GetHeaderCtrl()->GetSafeHwnd() ) );
}


BOOL CSortListCtrl::SetHeadings( UINT uiStringID )
{
	CString strHeadings;
	VERIFY( strHeadings.LoadString( uiStringID ) );
	return SetHeadings( strHeadings );
}


// the heading text is in the format column 1 text,column 1 width;column 2 text,column 3 width;etc.
BOOL CSortListCtrl::SetHeadings( const CString& strHeadings )
{
	m_ctlHeader.ClearColTextBuf();
	int iStart = 0;
	//	m_ctrlFileList.SetHeadings( _T("文件名,180;大小,150;修改日期,180;") );
	CString str = strHeadings;
	int iPos = str.Find(TEXT(","));
	while(iPos>0)
	{
		m_ctlHeader.SetHeadText(str.Left(iPos));		
		str = str.Right(str.GetLength()-iPos-1);
		iPos = str.Find(TEXT(";"));
		str = str.Right(str.GetLength()-iPos-1);
		iPos = str.Find(TEXT(","));
	}
	for( ;; )
	{
		const int iComma = strHeadings.Find( _T(','), iStart );

		if( iComma == -1 )
			break;

		const CString strHeading = strHeadings.Mid( iStart, iComma - iStart );

		iStart = iComma + 1;

		int iSemiColon = strHeadings.Find( _T(';'), iStart );

		if( iSemiColon == -1 )
			iSemiColon = strHeadings.GetLength();

		const int iWidth = _ttoi( strHeadings.Mid( iStart, iSemiColon - iStart ) );
		
		iStart = iSemiColon + 1;
	
		if( InsertColumn( m_iNumColumns++, strHeading, LVCFMT_LEFT, iWidth ) == -1 )
		{
			
			return FALSE;
		}		
		
	}

	return TRUE;
}

BOOL CSortListCtrl::SetTextArray( int iItem, LPTSTR* arrpsz )
{
	ASSERT( CListCtrl::GetItemData( iItem ) == NULL );
	ItemData* pid = new ItemData;
	pid->arrpsz = arrpsz;
	return CListCtrl::SetItemData( iItem, reinterpret_cast<DWORD>( pid ) );

}

void CSortListCtrl::DeleteAllCol()
{
	for(int i= m_iNumColumns; i >= 0; i--)
	{
		DeleteColumn(i);
	}
	m_iNumColumns = 0;
}
void CSortListCtrl::DeleteCol(int col)
{
	DeleteColumn(col);
	--m_iNumColumns;
}

int CSortListCtrl::AddItem(int imag, LPCTSTR pszText, ... )
{
	const int iIndex = InsertItem( GetItemCount(), pszText ,imag);

	LPTSTR* arrps = new LPTSTR[ m_iNumColumns ];
	arrps[ 0 ] = new TCHAR[ lstrlen( pszText ) + 1 ];

	(void)lstrcpy( arrps[ 0 ], pszText );

 	va_list list;
	va_start( list, pszText );

	for( int iColumn = 1; iColumn < m_iNumColumns; iColumn++ )
	{
		pszText = va_arg( list, LPCTSTR );
		ASSERT_VALID_STRING( pszText );
		VERIFY( CListCtrl::SetItem( iIndex, iColumn, LVIF_TEXT, pszText, 0, 0, 0, 0 ) );
		arrps[ iColumn ] = new TCHAR[ lstrlen( pszText ) + 1 ];
		(void)lstrcpy( arrps[ iColumn ], pszText );
	}
	/*pItem = arrps;*/
	va_end( list );

	VERIFY( SetTextArray( iIndex, arrps ) );

	return iIndex;
}


void CSortListCtrl::FreeItemMemory( const int iItem )
{
	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( iItem ) );

	LPTSTR* arrpsz = pid->arrpsz;

	for( int i = 0; i < m_iNumColumns; i++ )
		delete[] arrpsz[ i ];

	delete[] arrpsz;
	delete pid;

	VERIFY( CListCtrl::SetItemData( iItem, NULL ) );
}


BOOL CSortListCtrl::DeleteItem_EX( int iItem )
{
	FreeItemMemory( iItem );
	return CListCtrl::DeleteItem( iItem );
}


BOOL CSortListCtrl::DeleteAllItems_EX()
{

	for( int iItem = 0; iItem < GetItemCount(); iItem ++ )
		FreeItemMemory( iItem );
    
	return CListCtrl::DeleteAllItems();
}


bool IsNumber( LPCTSTR pszText )
{
	ASSERT_VALID_STRING( pszText );

	for( int i = 0; i < lstrlen( pszText ); i++ )
		if( !_istdigit( pszText[ i ] ) )
			return false;

	return true;
}


int NumberCompare( LPCTSTR pszNumber1, LPCTSTR pszNumber2 )
{
	ASSERT_VALID_STRING( pszNumber1 );
	ASSERT_VALID_STRING( pszNumber2 );

	const int iNumber1 = _ttoi( pszNumber1 );
	const int iNumber2 = _ttoi( pszNumber2 );

	if( iNumber1 < iNumber2 )
		return -1;
	
	if( iNumber1 > iNumber2 )
		return 1;

	return 0;
}


bool IsDate( LPCTSTR pszText )
{
	ASSERT_VALID_STRING( pszText );

	// format should be 99/99/9999.

	if( lstrlen( pszText ) != 10 )
		return false;

	return _istdigit( pszText[ 0 ] )
		&& _istdigit( pszText[ 1 ] )
		&& pszText[ 2 ] == _T('/')
		&& _istdigit( pszText[ 3 ] )
		&& _istdigit( pszText[ 4 ] )
		&& pszText[ 5 ] == _T('/')
		&& _istdigit( pszText[ 6 ] )
		&& _istdigit( pszText[ 7 ] )
		&& _istdigit( pszText[ 8 ] )
		&& _istdigit( pszText[ 9 ] );
}


int DateCompare( const CString& strDate1, const CString& strDate2 )
{
	const int iYear1 = _ttoi( strDate1.Mid( 6, 4 ) );
	const int iYear2 = _ttoi( strDate2.Mid( 6, 4 ) );

	if( iYear1 < iYear2 )
		return -1;

	if( iYear1 > iYear2 )
		return 1;

	const int iMonth1 = _ttoi( strDate1.Mid( 3, 2 ) );
	const int iMonth2 = _ttoi( strDate2.Mid( 3, 2 ) );

	if( iMonth1 < iMonth2 )
		return -1;

	if( iMonth1 > iMonth2 )
		return 1;

	const int iDay1 = _ttoi( strDate1.Mid( 0, 2 ) );
	const int iDay2 = _ttoi( strDate2.Mid( 0, 2 ) );

	if( iDay1 < iDay2 )
		return -1;

	if( iDay1 > iDay2 )
		return 1;

	return 0;
}


int CALLBACK CSortListCtrl::CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData )
{
	CSortListCtrl* pListCtrl = reinterpret_cast<CSortListCtrl*>( lParamData );
	ASSERT( pListCtrl->IsKindOf( RUNTIME_CLASS( CListCtrl ) ) );

	ItemData* pid1 = reinterpret_cast<ItemData*>( lParam1 );
	ItemData* pid2 = reinterpret_cast<ItemData*>( lParam2 );

	ASSERT( pid1 );
	ASSERT( pid2 );

	LPCTSTR pszText1 = pid1->arrpsz[ pListCtrl->m_iSortColumn ];
	LPCTSTR pszText2 = pid2->arrpsz[ pListCtrl->m_iSortColumn ];

	ASSERT_VALID_STRING( pszText1 );
	ASSERT_VALID_STRING( pszText2 );

	if( IsNumber( pszText1 ) )
		return pListCtrl->m_bSortAscending ? NumberCompare( pszText1, pszText2 ) : NumberCompare( pszText2, pszText1 );
	else if( IsDate( pszText1 ) )
		return pListCtrl->m_bSortAscending ? DateCompare( pszText1, pszText2 ) : DateCompare( pszText2, pszText1 );
	else
		// text.
		return pListCtrl->m_bSortAscending ? lstrcmp( pszText1, pszText2 ) : lstrcmp( pszText2, pszText1 );
}


void CSortListCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int iColumn = pNMListView->iSubItem;

	// if it's a second click on the same column then reverse the sort order,
	// otherwise sort the new column in ascending order.
	Sort( iColumn, iColumn == m_iSortColumn ? !m_bSortAscending : TRUE );

	*pResult = 0;
}


void CSortListCtrl::Sort( int iColumn, BOOL bAscending )
{
	m_iSortColumn = iColumn;
	m_bSortAscending = bAscending;

	// show the appropriate arrow in the header control.
	m_ctlHeader.SetSortArrow( m_iSortColumn, m_bSortAscending );

	VERIFY( SortItems( CompareFunction, reinterpret_cast<DWORD>( this ) ) );
}

void CSortListCtrl::InitHeadColor(COLORREF headerColor, COLORREF lineColor)
{
	m_ctlHeader.SetHeadBkColor(headerColor, lineColor);
	int itemCount = m_ctlHeader.GetItemCount();
	if(itemCount > 0)
	{
		for(int i = 0; i < itemCount; i++)
		{
			Sort(i);
		}
	}
	
}

void CSortListCtrl::LoadColumnInfo()
{
	// you must call this after setting the column headings.
	ASSERT( m_iNumColumns > 0 );

	CString strKey;
	strKey.Format( _T("%d"), GetDlgCtrlID() );

	UINT nBytes = 0;
	BYTE* buf = NULL;
	if( AfxGetApp()->GetProfileBinary( g_pszSection, strKey, &buf, &nBytes ) )
	{
		if( nBytes > 0 )
		{
			CMemFile memFile( buf, nBytes );
			CArchive ar( &memFile, CArchive::load );
			m_ctlHeader.Serialize( ar );
			ar.Close();

			m_ctlHeader.Invalidate();
		}

		delete[] buf;
	}
}


void CSortListCtrl::SaveColumnInfo()
{
	ASSERT( m_iNumColumns > 0 );

	CString strKey;
	strKey.Format( _T("%d"), GetDlgCtrlID() );

	CMemFile memFile;

	CArchive ar( &memFile, CArchive::store );
	m_ctlHeader.Serialize( ar );
	ar.Close();

	DWORD dwLen = memFile.GetLength();
	BYTE* buf = memFile.Detach();	

	VERIFY( AfxGetApp()->WriteProfileBinary( g_pszSection, strKey, buf, dwLen ) );

	free( buf );
}


/*void CSortListCtrl::OnDestroy_EX() 
{
	for( int iItem = 0; iItem < GetItemCount(); iItem ++ )
		FreeItemMemory( iItem );

	CListCtrl::OnDestroy();
}
*/

BOOL CSortListCtrl::SetItemText_EX( int nItem, int nSubItem, LPCTSTR lpszText )
{
	if( !CListCtrl::SetItemText( nItem, nSubItem, lpszText ) )
		return FALSE;

	LPTSTR* arrpsz = GetTextArray( nItem );
	LPTSTR pszText = arrpsz[ nSubItem ];
	delete[] pszText;
	pszText = new TCHAR[ lstrlen( lpszText ) + 1 ];
	(void)lstrcpy( pszText, lpszText );
	arrpsz[ nSubItem ] = pszText;

	return TRUE;
}


BOOL CSortListCtrl::SetItemData_EX( int nItem, DWORD dwData )
{
	if( nItem >= GetItemCount() )
		return FALSE;

	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( nItem ) );
	ASSERT( pid );
	pid->dwData = dwData;

	return TRUE;
}


DWORD CSortListCtrl::GetItemData_EX( int nItem ) const
{
	ASSERT( nItem < GetItemCount() );
	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( nItem ) );
	ASSERT( pid );
	return pid->dwData;
}

DWORD CSortListCtrl::GetItemData_( int nItem ) const
{
	return GetItemData(nItem);
}



LPTSTR* CSortListCtrl::GetTextArray( int iItem ) const
{
	ASSERT( iItem < GetItemCount() );

	ItemData* pid = reinterpret_cast<ItemData*>( CListCtrl::GetItemData( iItem ) );
	return pid->arrpsz;
}

void CSortListCtrl::SelectItem(int nItem,bool bSetVisible)
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

void CSortListCtrl::SetItemIconIndex(int nItem,int nIndex)
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

void CSortListCtrl::EnableDbItemColor(bool bEnable,COLORREF clr)
{
	m_bEnableDbItemColor = bEnable ;
	m_clrDbItemBK = clr ;
}

void CSortListCtrl::SetHotAllItem(bool bHotItem)
{
	m_bHotAllItem = bHotItem ;
}

 
void CSortListCtrl::SetSubitemCursor(bool bSetFlag)
{
	m_bSubItemCursor = bSetFlag;
}

void CSortListCtrl::OnDestroy() 
{	
	for( int iItem = 0; iItem < GetItemCount(); iItem ++ )
	{
		FreeItemMemory( iItem );		
	}

	// TODO: Add your message handler code here
	CListCtrl::OnDestroy();
}

