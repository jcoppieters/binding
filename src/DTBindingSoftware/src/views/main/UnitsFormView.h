#if !defined(AFX_UNITSFORMVIEW_H__AC192861_5190_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_UNITSFORMVIEW_H__AC192861_5190_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitsFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnitsFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define IDC_LISTVIEW_UNITS		80003


class CCanNode;
#include "MyNodeUnitListCtrl.h"


///////////////////////////////////////////////////////////////////

#define NODELIST_SIZE	5

class DisplayNodesList
{
public:
	DisplayNodesList();

private:	
	BYTE NodeList[NODELIST_SIZE];
	BYTE bIndex;
	BYTE bCount;

	void FragmentList();

public:
	BOOL AddToList(BYTE bNodeAddress);
	BOOL RemoveFromList(BYTE bNodeAddress);
	BYTE Count();
	BOOL Get(BYTE bItemIndex,BYTE *bAddress);
	void Init(void);
};


///////////////////////////////////////////////////////////////////
class CUnitsFormView : public CFormView
{
protected:
	CUnitsFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CUnitsFormView)

// Form Data
public:
	//{{AFX_DATA(CUnitsFormView)
	enum { IDD = IDD_UNITSFORMVIEW_FORM };
	CStatic	m_StaticCtrl;
	//}}AFX_DATA

// Attributes
public:
// Operations


protected:
	DisplayNodesList NodeList;
	CMyNodeUnitListCtrl	*pListView;

	BOOL fControlsCreated;

	void InitControls(void);
	void CreateUnitListView(BYTE bLogicalAddress);
	void AddUnitsToListView(CCanNode *pNode);
	void DisplayUnits(void);
	BOOL ShowNodeInUnitListView(BYTE bLogicalAddress);	
	BOOL GetUnitItem(BYTE bLogicalAddress,BYTE bUnitAddress,int *iItem);

	// Interfaces
	LONG OnShowUnitsForm(WPARAM wParam,LPARAM lParam);	
	LONG OnUnitStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG OnUnitNameChanged(WPARAM wParam,LPARAM lParam);
	LONG OnTreeNodeCollapsed(WPARAM wParam,LPARAM lParam);
	LONG OnTreeNodeExpanded(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);

public:
	void DisplayStats();
	//void UnitStatusChanged(BYTE bNodeLogicalAddress, BYTE bUnitNr);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitsFormView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CUnitsFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CUnitsFormView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITSFORMVIEW_H__AC192861_5190_11D8_B865_0050BAC412B1__INCLUDED_)
