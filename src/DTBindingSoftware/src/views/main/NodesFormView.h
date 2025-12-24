#if !defined(AFX_NODESFORMVIEW_H__B99D1501_4C4A_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_NODESFORMVIEW_H__B99D1501_4C4A_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NodesFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNodesFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#define IDC_LISTVIEW		80002

//#include "NodeDatabase.h"

#include "Mainfrm.h"
#include "MyNodeUnitListCtrl.h"
class CCanNode;


class CNodesFormView : public CFormView
{
protected:
	CNodesFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNodesFormView)
	// static BOOL m_fSortMethod[6];

// Form Data
public:
	//{{AFX_DATA(CNodesFormView)
	enum { IDD = IDD_NODESFORMVIEW_FORM };
	CStatic	m_StaticCtrl;
	//}}AFX_DATA

protected:
	CMyNodeUnitListCtrl* m_pListView;
	CMainFrame* m_pMainWnd;	
	BOOL m_fControlsCreated;

protected:
	void CreateNodeListView();
	void AddNodeToListView(CCanNode *pNode,int EntryNr);	
	void UpdateView(void);

	LONG OnShowNodesForm(WPARAM wParam,LPARAM lParam);	
	LONG OnUpdateNodesForm(WPARAM wParam,LPARAM lParam);
	LONG OnNodeNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnNodeStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);
	LONG onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam);		// sedert V16.03

// Attributes
public:

// Operations
public:
	void DisplayInstallationStats(void);
	void CreateUnitListView(BYTE bLogicalAddress);
	void AddUnitsToListView(CCanNode *pNode);
	void InitControls(void);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNodesFormView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL GetNodeItem(BYTE bLogicalAddress,int *iItem);
	virtual ~CNodesFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CNodesFormView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NODESFORMVIEW_H__B99D1501_4C4A_11D8_B865_0050BAC412B1__INCLUDED_)
