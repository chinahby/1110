//********************************************
//	Helper functions to emulate user input
//********************************************
#ifndef WINDOWS_OPREATION_FUNC_672B8EB3143548f2BD06F1B2CEE96899
#define WINDOWS_OPREATION_FUNC_672B8EB3143548f2BD06F1B2CEE96899

#include <vector>
using std::vector;

/**	Found a window by its caption.
*	Found all topmost window in the desktop.
*
*	@param strTitle[in]
*		the window caption to be found.
*	@param dwWaitTime[in]
*		the time we wait for the window appearence		
*	@return 
*		The found  window's handle or NULL if the window not found		
*/
HWND FindWindowTM(const CString& strTitle, DWORD dwWaitTime);

/**	Enumerate all visible child window of a window
*	Enumerate all visible child window of a window and put their
*	window handles into a vector.
*
*	@param pWnd[in]
*		the pointer to the parent window
*	@param childWnds
*		vector to retreive child window handles
*/
void GetChildeWnds(CWnd* pWnd, vector<HWND>& childWnds);

/**	Find a child window by its windows text.
*	Find a child window by compare its text, case insensitive.
*
*	@param pWnd[i]
*		pointer to parent window
*	@param strChildWndName[i]
*		the child window's text to be found
*	@return
*		the handle of the found child window,
*		or NULL if the child window not found.
*/
HWND GetChildWndByName(CWnd* pWnd, const CString& strChildWndName);

/**	Get a windows class name.
*
*	@param hwnd[in]
*		handle of the window.
*	@param className[out]
*		string to retrieve class name.
*/
void GetWndClassName(HWND hwnd, CString& className);

/** Check if a window's class name is matched the specified name.
*
*	@param hwnd[in]
*		the winddow handle.
*	@param className[in]
*		the spcified class name to be matched.
*/
bool MatchClass(HWND hwnd, const CString& className);

/**	Find all child windows that match a class name.
*	Find all child windows that match a class name, and put heir handle
*	into a vector.
*
*	@param pWnd[in]
*		the parent window
*	@param className[in]
*		the specified window class name.
*	@param childWnds[out]
*		vector to retreive the child windows' handle.
*
*/
void GetChildWndByClassName(CWnd* pWnd, const CString& className, vector<HWND>& childWnds);

/**	Set the text of a Combobox.
*
*	@param pWnd [in]
*		pointer to the Combobox.
*	@param strText [in]
*		the new text to be set
*	@return 
*		if the pWnd is a combobox, return true,
*		else return false.
*/
bool SetComboText(CWnd* pWnd, const CString& strText);

/** Click a child window
*	Send WM_LBUTTONDWON and WM_LBUTTONUP message to emulate
*	a click action on a child window.
*	
*	@param pWnd [in]
*		the parent window
*	@param strChildWndName [in]
*		the child window's text that used to find the child window.
*	@return
*		if the child not found, return false,
*		else return true.
*/
bool ClickChildWnd(CWnd* pWnd, const CString& strChildWndName);

void ClickWindow(HWND hwnd);

/**	Set a childe window's check status.
*	The child window must be a check button.
*
*	@param pWnd [in]
*		parent window.
*	@param strBtnWndName [in]
*		the button name.
*	@param bCheck [in]
*		Set check or not.
*	@return
*		if the child window is a button, return true,
*		else return false.
*/
bool CheckChildWnd(CWnd* pWnd, const CString& strBtnWndName, bool bCheck);

bool CheckWindow(HWND hwnd, bool bChecked);


/**	Find a string in all child windows.
*	To check if the specified string exsting in current visible child windows.
*	
*	@param pWnd [in]
*		the parent window
*	@param str [in]
*		the string to be found
*	@return
*		if the string contained in a window's text, 
*		return the window's handle, else return NULL
*/
HWND FindStringInChildWnds(CWnd* pWnd, const CString& str);

/** Check if the caption contains a specified string.
*	
*	@param hwnd [in]
*		the window handle
*	@param str [in]
*		the string to be found.
*	@return
*		If the string is contained in the windows caption, 
*		return true, otherwise return false
*/
bool ContainText(HWND hwnd, const CString& str);

/** Find top level window that match a condition.
*	
*	@param condition
*		the condition functions pointer.
*/
typedef bool (*TCondition)(HWND hwnd);
HWND FindWnd_If(TCondition condition);

/** Get the text of a window.
*
*/
CString GetWindowText(HWND hwnd);

#endif