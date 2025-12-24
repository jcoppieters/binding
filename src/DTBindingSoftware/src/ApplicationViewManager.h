// ApplicationViewManager.h: interface for the CApplicationViewManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATIONVIEWMANAGER_H__5826ED82_B9F2_11D8_8499_0050BAC412B1__INCLUDED_)
#define AFX_APPLICATIONVIEWMANAGER_H__5826ED82_B9F2_11D8_8499_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "uicontrols/panels/MySplitterWnd.h"

class CMyScrollView;
class CMyFrameWnd;
class CMyFormView;

class CApplicationViewManager  
{
public:
	static CApplicationViewManager* GetInstance();

public:
	CApplicationViewManager();
	virtual ~CApplicationViewManager();

	CMySplitterWnd	m_wndSplitter;
	CMyScrollView	*pScrollView;
	CMyFrameWnd		*pMyFrameWnd;
	CMyFormView		*pMyTreeView;

public:
	BOOL CreateMainSplitter(CWnd *pMainWnd,CCreateContext* pContext);

	BOOL IsBindingViewActive(void);
	BOOL IsNodesViewActive(void);
	BOOL IsUnitsViewActive(void);

	BOOL IsTreeNodesActive(void);
	BOOL IsTreeBindingsActive(void);
	BOOL IsTreeTemplatesActive(void);

	LONG OnNodeNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnUnitNameChanged(WPARAM wParam,LPARAM lParam);

	LONG OnDisplayMessage(WPARAM wParam,LPARAM lParam=0);
	LONG OnClearMessages(WPARAM wParam=0,LPARAM lParam=0);

	LONG OnUnitStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG OnNodeStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG onNodeEthernetNwkStatus(WPARAM wParam,LPARAM lParam);		// sedert V16.03

	LONG OnTreeNodeCollapsed(WPARAM wParam,LPARAM lParam);
	LONG OnTreeNodeExpanded(WPARAM wParam,LPARAM lParam);

	LONG OnAddFlowAddUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowDraggingUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowDropUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowRemoveControl(WPARAM wParam,LPARAM lParam);

	LONG OnBindingEntryAdd(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryDelete(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryEdit(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryNew(WPARAM wParam=0,LPARAM lParam=0);

	LONG OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnBindingdatabaseUpdate(WPARAM wParam=0,LPARAM lParam=0);

	void OnButtonAnd();
	void OnButtonOr();
	void OnButtonTimer();
	void OnButtonXor();

	void ShowNodesForm();
	void ShowBindingsForm(void);
	void ShowUnitsForm(void);

	BOOL InitViews(void);
	void ClearViews(void);
	void UpdateViews(int nodeAddress);

	WPARAM GetBindingEntry(void);
	CString GetEditControlBindingName(void);

	void OnViewSortnodesbyname();
	void OnViewSortnodesbyphaddress();
	void OnViewSortnodesbylogicaladdress();
	void OnViewSortnodesonlyname();
	void OnButtonSortNodes();
	void OnButtonCollapse();
	void OnButtonExpand();
	void OnViewSortBindings();
	void OnButtonSortUnits();
	void OnViewUnitsbyaddress();
	void OnViewUnitsbyname();
	void OnViewUnitsonlyname();
	void OnTreeviewBindingsNameandnumber();
	void OnTreeviewBindingsNumberandname();
	void OnTreeviewBindingsOnlyname();
};

#endif // !defined(AFX_APPLICATIONVIEWMANAGER_H__5826ED82_B9F2_11D8_8499_0050BAC412B1__INCLUDED_)
