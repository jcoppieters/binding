#if !defined(AFX_DLGBINDINGBOSEV35UNIT_H__56ACAFDF_7A73_4CD6_83B6_9F1C6930E1A7__INCLUDED_)
#define AFX_DLGBINDINGBOSEV35UNIT_H__56ACAFDF_7A73_4CD6_83B6_9F1C6930E1A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingBoseV35Unit.h : header file
//

#include "DlgBindingUnitProp.h"
#include "BindingUIElement.h"

class CBindingBoseV35Unit;


/////////////////////////////////////////////////////////////////////////////
// CDlgBindingBoseV35Unit dialog

class CDlgBindingBoseV35Unit : public CDlgBindingUnitProp
{
protected:
	CBindingBoseV35Unit *m_pBindingBoseV35Unit;
	CBindingBoseV35Unit_Helper::CIteratorBindingFunctions* m_pIteratorBindingFunctions; /* Helper voor weergaven van de bindingvlag functies */
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;


// Construction
public:
	CDlgBindingBoseV35Unit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingBoseV35Unit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingBoseV35Unit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingBoseV35Unit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);
	virtual void OnSpin3DataChanged(int iStep);
	void UpdateControls(void);
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGBOSEV35UNIT_H__56ACAFDF_7A73_4CD6_83B6_9F1C6930E1A7__INCLUDED_)
