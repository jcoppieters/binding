#if !defined(AFX_MYFRAMEWND_H__C07780E1_4E7F_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYFRAMEWND_H__C07780E1_4E7F_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyFrameWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyFrameWnd frame

class CNodesFormView;
class CMyAddFlowSheet;
class CUnitsFormView;

enum ACTIVE_VIEW
{
	ACTIVE_VIEW_BINDINGS,
	ACTIVE_VIEW_NODES,
	ACTIVE_VIEW_UNITS
};

class CMyFrameWnd : public CFrameWnd
{
	ACTIVE_VIEW ActiveView;
	
	DECLARE_DYNCREATE(CMyFrameWnd)
protected:
	CMyFrameWnd();           // protected constructor used by dynamic creation

	//CToolBar my_wndToolBar;

// Attributes
public:
	CNodesFormView *pNodesFormView;
	CMyAddFlowSheet *pAddFormView;
	CUnitsFormView *pUnitsFormView;

	ACTIVE_VIEW GetCurrentActiveView(void) {return ActiveView; }

// Operations
public:
	//void UnitStatusChanged(BYTE bNodeLogicalAddress,BYTE bUnitNr);
	void ShowUnitsForm(BYTE bNodeAddress,BOOL fBool);
	void UpdateAllViews(void);
	void InitViews(void);
	void ShowAddFlowForm(BOOL fBool);
	void ShowNodesForm(BOOL fBool);
	//BOOL CreateViews(CCreateContext* pContext);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyFrameWnd)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(CMyFrameWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LONG OnNodeNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnUnitNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnUnitStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG OnNodeStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam);		// sedert V16.03
	LONG OnTreeNodeCollapsed(WPARAM wParam,LPARAM lParam);
	LONG OnTreeNodeExpanded(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);
	LONG OnTreeviewCollapse(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYFRAMEWND_H__C07780E1_4E7F_11D8_B865_0050BAC412B1__INCLUDED_)
