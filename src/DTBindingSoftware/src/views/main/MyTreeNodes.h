#if !defined(AFX_MYTREENODES_H__EB3998A1_8312_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYTREENODES_H__EB3998A1_8312_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTreeNodes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyTreeNodes window
class CCanNode;
struct STRUCT_UNIT;
class CBindingEntry;


////////////////////////////////////////////////////////////////////////////
class CDraggingItem
{
public:
	typedef enum {
		STATE_IDLE = 0,
		STATE_WAIT,
		STATE_BUSY
	} State_t;

	CDraggingItem( );
	~CDraggingItem( );

	void setIdle( );
	void setWait(CPoint& pos);
	void setBusy(CImageList* pImageList);

	State_t getState() const {
		return m_state;
	}	
	CImageList* getImageList( );
	const CPoint& getPoint() const {
		return m_point;
	}

protected:
	void destroyImageList();

private:
	State_t m_state;
	CImageList* m_pImageList;
	CPoint m_point;
};
////////////////////////////////////////////////////////////////////////////

class CMyTreeNodes : public CTreeCtrl
{
	typedef enum 
	{
		NODES_VIEWMODE_NAME = 0,
		NODES_VIEWMODE_PHADDRESS,
		NODES_VIEWMODE_LOGADDRESS,
		NODES_VIEWMODE_ONLYNAME			/* TM 20040715*/
	
	}NODES_VIEW_MODE;

	typedef enum 
	{
		UNITS_VIEWMODE_NAME = 0,
		UNITS_VIEWMODE_LOGADDRESS,
		UNITS_VIEWMODE_ONLYNAME
	
	}UNITS_VIEW_MODE;

	enum { 
		WM_TIMER_DRAG = 1000,
		WM_TIMER_EDITNAME				// zie WORKAROUND_LABEL_EDIT
	};

	
	void AddNode(HTREEITEM parent,HTREEITEM *pNewParent,CCanNode *pNode);
	void AddUnit(HTREEITEM parent,HTREEITEM *pNewParent,STRUCT_UNIT *pUnit);

//	void AddUnit(HTREEITEM parent,HTREEITEM *pNewParent,STRUCT_UNIT Unit);
	void AddBindingEntry(HTREEITEM parent,HTREEITEM *pNewParent,CBindingEntry *pEntry);
	BOOL AddBindingEntry(BYTE bNodeAddress,BYTE bUnitAddress,CBindingEntry *pEntry);
	HTREEITEM GetBindingEntry(unsigned short nBindingID,int nCnt);
	BOOL DeleteBindingEntry(unsigned short nBindingID);
	//BOOL UpdateBindingEntry(unsigned short nBindingID);
	BOOL UpdateBindingEntry(CBindingEntry* const pEntry);
	HTREEITEM GetUnit(BYTE bNodeAddress,BYTE bUnitAddress,HTREEITEM parent);
	HTREEITEM GetNode(BYTE bLogicalAddress);
	STRUCT_UNIT* GetUnit(HTREEITEM treeItem);
	BOOL DisplayNodeName(HTREEITEM hItem,CCanNode *pNode);
	BOOL DisplayUnitName(HTREEITEM hItem,STRUCT_UNIT *pUnit);	/* TM 20040716*/
	BOOL SetBindingEntryIcon(HTREEITEM parent,HTREEITEM current,CBindingEntry *pEntry);
	BOOL UpdateNodeNames(void);
	BOOL UpdateUnitNames(void);

	typedef void (CMyTreeNodes::*_CALLBACK)(HTREEITEM* hBindingEntr);

	void UpdateBindingEntry(HTREEITEM* hBindingEntr);
	void IterateBindingEntries(_CALLBACK fc);

	// Mogelijkheid om een individuele node up te daten.		<------ FUNCTIE TESTEN!!
	void updateSingleNode(BYTE bNodeAddress);


// Construction
public:
	CMyTreeNodes();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTreeNodes)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Update();
	virtual ~CMyTreeNodes();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTreeNodes)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);
	afx_msg void OnContextmenuUnitRename();
	afx_msg void OnContextmenuShowUnitProperties();
	afx_msg void OnContextmenuNodeRename();
	afx_msg void OnContextmenuNodeShowProperties();
	afx_msg void OnContextmenuNodeShowNodeControl();
	afx_msg void OnContextmenuNodeShowNodeSmap();
	afx_msg void OnContextmenuNodeShowNodeConfig();
	afx_msg void OnContextmenuNodeShowNodeMasterTasks();
	afx_msg void OnContextmenuBindingsRename();
	afx_msg void OnContextmenuBindingsShow();
	afx_msg void OnContextmenuShowLanCfg();
	afx_msg void OnContextmenuNodeBrowse();
	afx_msg void OnMyTreeGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	LONG OnNodeNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnUnitNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnUnitStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG OnBindingDatabaseAddEntry(WPARAM wParam,LPARAM lParam);
	LONG OnBindingDatabaseRemoveEntry(WPARAM wParam,LPARAM lParam);
	LONG OnBindingDatabaseNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnBindingdatabaseUpdate(WPARAM wParam,LPARAM lParam);

	// New
	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam);
	//New
	LONG OnTreeViewSortNodesByName(WPARAM wParam,LPARAM lParam);
	LONG OnViewSortnodesbyphaddress(WPARAM wParam,LPARAM lParam);
	LONG OnViewSortnodesbylogicaladdress(WPARAM wParam,LPARAM lParam);
	LONG OnButtonSortNodes(WPARAM wParam,LPARAM lParam);
	LONG OnViewSortNodesonlyname(WPARAM wParam,LPARAM lParam);
	LONG OnButtonCollapse(WPARAM wParam,LPARAM lParam);
	LONG OnButtonExpand(WPARAM wParam,LPARAM lParam);	
	LONG OnButtonSortUnits(WPARAM wParam,LPARAM lParam);
	LONG OnViewUnitsbyname(WPARAM wParam,LPARAM lParam);
	LONG OnViewUnitsbyaddress(WPARAM wParam,LPARAM lParam);
	LONG OnViewUnitsonlyname(WPARAM wParam,LPARAM lParam);

	enum TypeOfTreeCtrl
	{
		TYPE_TREE_NO_CTRL = 0, 
		TYPE_TREE_NODE_CTRL = 1, 
		TYPE_TREE_UNIT_CTRL = 2,
		TYPE_TREE_BINDING_CTRL = 3
	};

	enum TreeTypes
	{	
		TYPE_TREE_NONE = 0, 
		TYPE_TREE_NODES = 1, 
		TYPE_TREE_BINDINGS = 2,
		TYPE_TREE_TEMPLATES = 3
	};

	void setCurrentSelectedItem(HTREEITEM hItem);
	BOOL getSelectedNode(HTREEITEM htreeItem,CCanNode** ppNode);
	BOOL getSelItemAddress(HTREEITEM htreeItem,BYTE* pbNodeAddress,BYTE* pbUnitAddress);
	TypeOfTreeCtrl getTypeCtrl(HTREEITEM htreeItem) const;
	void openContextMenu(CWnd* pWnd, CPoint ptMousePos,HTREEITEM hSelectedItem);
	void openUnitContextMenu(CWnd* pWnd, CPoint ptMousePos);
	void openNodeContextMenu(CWnd* pWnd, CPoint ptMousePos);
	void openBindingContextMenu(CWnd* pWnd, CPoint ptMousePos);
	

	DECLARE_MESSAGE_MAP()

private:
	NODES_VIEW_MODE NodesViewMode;
	UNITS_VIEW_MODE UnitsViewMode;
	CFont *pTreefont;
	CDraggingItem m_draggingItem;
	HTREEITEM m_hCurrentItemSelected;				
	BYTE m_popupSelectedNodeAddress;				// Sedert V16.35: Selected node bijhouden via het node address (zie ook BUG-0176)
	unsigned long m_popupSelectedItemDataValue;		// Sedert V16.35: Selected item data bijhouden van het geselecteerde item.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTREENODES_H__EB3998A1_8312_11D8_B865_0050BAC412B1__INCLUDED_)
