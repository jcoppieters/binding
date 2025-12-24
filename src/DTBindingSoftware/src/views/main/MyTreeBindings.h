#if !defined(AFX_MYTREEBINDINGS_H__EB3998B3_8312_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYTREEBINDINGS_H__EB3998B3_8312_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTreeBindings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyTreeBindings window
class CMainFrame;
class CBindingEntry;

class CMyTreeBindings : public CTreeCtrl
{
	void AddBindingEntry(HTREEITEM parent,HTREEITEM *pNewParent,CBindingEntry *pEntry);
	BOOL AddBindingEntry(unsigned short nID);
	BOOL DeleteEntry(unsigned short nID);
	HTREEITEM GetBindingEntry(unsigned short nID);
	BOOL DisplayBindingName(HTREEITEM hItem,CBindingEntry *pEntry);

// Construction
public:
	CMyTreeBindings();

// Attributes
public:
	BOOL Update();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTreeBindings)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyTreeBindings();

	// Generated message map functions
protected:
	
	//{{AFX_MSG(CMyTreeBindings)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);
	afx_msg void OnContextmenuBindingsRename();
	afx_msg void OnContextmenuBindingsShow();
	// afx_msg void OnContextmenuBindingsDelete();
	//}}AFX_MSG

	LONG OnBindingdatabaseUpdate(WPARAM wParam,LPARAM lParam);
	LONG OnBindingDatabaseAdd(WPARAM wParam,LPARAM lParam);
	LONG OnBindingDatabaseRemove(WPARAM wParam,LPARAM lParam);
	LONG OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam);	
	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam);
	LONG OnViewSortBindings(WPARAM wParam,LPARAM lParam);
	LONG OnTreeViewSortBindingsOnlyName(WPARAM wParam,LPARAM lParam);
	LONG OnTreeViewSortBindingsName(WPARAM wParam,LPARAM lParam);
	LONG OnTreeViewSortBindingsNumber(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()


	void showViewBindings();
	void RequestEditBinding(HTREEITEM hItem);

#if(0)
	void RequestDeleteBinding(HTREEITEM hItem);
#endif 

private:
	typedef enum 
	{
		BINDINGS_VIEWMODE_ONLYNAME = 0,
		BINDINGS_VIEWMODE_NAME,
		BINDINGS_VIEWMODE_BINDINGNR	
	
	}BINDINGS_VIEW_MODE;
	
	BINDINGS_VIEW_MODE m_bindingViewMode;
	CMainFrame* m_pMainWnd;
	HTREEITEM m_hActiveItem;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTREEBINDINGS_H__EB3998B3_8312_11D8_B865_0050BAC412B1__INCLUDED_)
