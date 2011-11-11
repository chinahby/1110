#ifndef SORTLISTCTRL_H
#define SORTLISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SORTHEADERCTRL_H
	#include "SortHeaderCtrl.h"
#endif	// SORTHEADERCTRL_H

#ifdef _DEBUG
	#define ASSERT_VALID_STRING( str ) ASSERT( !IsBadStringPtr( str, 0xfffff ) )
#else	//	_DEBUG
	#define ASSERT_VALID_STRING( str ) ( (void)0 )
#endif	//	_DEBUG


class CSortListCtrl : public CListCtrl
{
// Construction
public:
	CSortListCtrl();

// Attributes
public:
   

// Operations
public:
	BOOL SetHeadings( UINT uiStringID );
	BOOL SetHeadings( const CString& strHeadings );
	void DeleteAllCol();
	void DeleteCol(int col);
	int AddItem(int imag, LPCTSTR pszText, ... );
	BOOL DeleteItem_EX( int iItem );
	BOOL DeleteAllItems_EX();
	void LoadColumnInfo();
	void SaveColumnInfo();
	BOOL SetItemText_EX( int nItem, int nSubItem, LPCTSTR lpszText );
	void Sort( int iColumn, BOOL bAscending = FALSE );
	BOOL SetItemData_EX(int nItem, DWORD dwData);
	DWORD GetItemData_EX(int nItem) const;
	DWORD GetItemData_( int nItem ) const;
	void SelectItem(int nItem,bool bSetVisible=false) ;
    void SetItemIconIndex(int nItem,int nIndex=-1);
	void InitHeadColor(COLORREF headerColor = RGB(215,215,215), COLORREF lineColor = RGB(210,210,210));
    int  m_nLSelItem  ;

	//hxb
	bool m_bEnableDbItemColor ; 
    bool m_bHotAllItem ;
	COLORREF m_clrDbItemBK  ;
	bool m_bSubItemCursor ;
	
	void EnableDbItemColor(bool bEnable=true,COLORREF clr=RGB(228,241,226));
	void SetHotAllItem(bool bHotItem);
	void SetSubitemCursor(bool bSetFlag=true);
	CRect GetFullCellRect(int nItem, BOOL outside);
	CRect GetCellRect(int nItem, int nColumn, BOOL outside);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSortListCtrl();

	// Generated message map functions
protected:
	static int CALLBACK CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData );
	void FreeItemMemory( const int iItem );
	BOOL CSortListCtrl::SetTextArray( int iItem, LPTSTR* arrpsz );
	LPTSTR* CSortListCtrl::GetTextArray( int iItem ) const;
	

	CSortHeaderCtrl m_ctlHeader;
	int m_iNumColumns;
	int m_iSortColumn;
	BOOL m_bSortAscending;

	//{{AFX_MSG(CSortListCtrl)
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTLISTCTRL_H
