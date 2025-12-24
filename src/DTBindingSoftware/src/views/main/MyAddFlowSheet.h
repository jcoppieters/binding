#if !defined(AFX_MYADDFLOWSHEET_H__18482981_4E8C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_MYADDFLOWSHEET_H__18482981_4E8C_11D8_B865_0050BAC412B1__INCLUDED_


#include "addflow\addflow1.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyAddFlowSheet.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CMyAddFlowSheet form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#include "BindingEntry.h"
#include "BindingConversion.h"
#include "BindingGuiComponent.h"
#include "BindingConversionToGui2.h"
#include "bindingUIElements.h"

#include "uicontrols/controls/MyEditCtrl.h"

/////////////////////////////////////////////////////////////

class CMyAddFlowSheet : public CFormView
{
protected:
	CMyAddFlowSheet();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyAddFlowSheet)

	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

protected:
	BOOL		fControlsCreated;
	CBindingGuiComponent BindingGuiComponent;
	CBindingEntry BindingView;

	void FreeProperties(void);
	void ShowProperties(void);	
	BOOL CreateNewBindingEntry(void);
	void UpdateControls(void);

	// Property Dialogs
	LONG OnAddFlowAddUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowAddControl(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowDraggingUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowDropUnit(WPARAM wParam,LPARAM lParam);
	LONG OnAddFlowRemoveControl(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryNew(WPARAM wParam,LPARAM lParam);
	LONG OnBindingEntryEdit(WPARAM wParam,LPARAM lParam);
	LONG OnUnitStatusChanged(WPARAM wParam,LPARAM lParam);
	LONG OnApplicationClearViews(WPARAM wParam,LPARAM lParam);
// Form Data
public:


	CBindingGuiComponent* GetBindingGuiComponent(void) 
	{
		return (&BindingGuiComponent); 
	};	
	const CBindingEntry* GetBindingView(void)	
	{
		return (&BindingView); 
	};
	CAddFlow1 *GetAddFlowComponent(void)		
	{ 
		return (&m_AddFlow); 
	};

	BOOL fDropping;

	void InitAddFlowDiagram2(void);

	//{{AFX_DATA(CMyAddFlowSheet)
	enum { IDD = IDD_MYADDFLOWSHEET_FORM };
	CMyEditCtrl	m_EditBindingName;
	CStatic	m_StaticNr;
	CSpinButtonCtrl	m_Spin1;
	CMyEditCtrl		m_EditSpin;
	CStatic			m_StaticName;
	CAddFlow1	m_AddFlow;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void InitAddFlowDiagram(void);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyAddFlowSheet)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LONG OnNotifyParentKeyEnter(WPARAM wParam,LPARAM lParam);
	virtual ~CMyAddFlowSheet();
	
	LONG OnShowAddflowForm(WPARAM wParam,LPARAM lParam);
	//LONG OnApplicationUpdateViews(WPARAM wParam,LPARAM lParam);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	// Generated message map functions
	//{{AFX_MSG(CMyAddFlowSheet)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseDownAddflowComponent(short Button, short Shift, long x, long y);
	afx_msg void OnMouseMoveAddflowComponent(short Button, short Shift, long x, long y);
	afx_msg void OnMouseUpAddflowComponent(short Button, short Shift, long x, long y);
	afx_msg void OnDblClickAddflowComponent();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditBindingname();
	afx_msg void OnKeyDownAddflowComponent(short FAR* KeyCode, short Shift);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYADDFLOWSHEET_H__18482981_4E8C_11D8_B865_0050BAC412B1__INCLUDED_)
