#if !defined(AFX_MYNODEUNITLISTCTRL_H__AC192865_5190_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYNODEUNITLISTCTRL_H__AC192865_5190_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyNodeUnitListCtrl.h : header file
//

class CCanNode;

/////////////////////////////////////////////////////////////////////////////
// CMyNodeUnitListCtrl window
class CMyNodeUnitListCtrl : public CListCtrl
{
	// Construction
public:
	enum ListTypes {
		TYPE_LIST_NONE = 0, TYPE_LIST_NODES, TYPE_LIST_UNITS
	};
	enum TypeOfListCtrl {
		TYPE_LIST_NO_CTRL = 0, TYPE_LIST_NODE_CTRL, TYPE_LIST_UNIT_CTRL
	};
	enum ListCtrlStates {
		LISTCTRL_STATE_VIEW_NODES, LISTCTRL_STATE_VIEW_UNITS
	};
	CMyNodeUnitListCtrl(ListCtrlStates myState);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyNodeUnitListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	unsigned long SetParamData(ListTypes x,TypeOfListCtrl y,BYTE NodeAddress,BYTE bUnitAddress);
	virtual ~CMyNodeUnitListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyNodeUnitListCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	// afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);
	afx_msg void OnContextmenunodeRename();
	afx_msg void OnContextmenuShowProperties();
	afx_msg void OnContextmenuShowNodeControl();
	afx_msg void OnContextmenuShowNodeSmap();
	afx_msg void OnContextmenuShowNodeConfig();
	afx_msg void OnContextmenuShowNodeMasterTasks();
	afx_msg void OnContextmenuShowLanCfg();
	afx_msg void OnContextmenuNodeBrowse();
	afx_msg void OnContextMenuUnitShowProperties();
	afx_msg void OnContextMenuUnitRename();
	//}}AFX_MSG 

	DECLARE_MESSAGE_MAP()

	BOOL getSelectedNode(int item,CCanNode** ppNode);
	BOOL getSelItemAddress(int item,BYTE* pbNodeAddress,BYTE* pbUnitAddress);
	void openNodeContextMenu(CWnd* pWnd, CPoint ptMousePos);
	void openUnitContextMenu(CWnd* pWnd, CPoint ptMousePos);

private:
	const ListCtrlStates m_CurrentState;
	int m_selectedItem;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYNODEUNITLISTCTRL_H__AC192865_5190_11D8_B865_0050BAC412B1__INCLUDED_)
