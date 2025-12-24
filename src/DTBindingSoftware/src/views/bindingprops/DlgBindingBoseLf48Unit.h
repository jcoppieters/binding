#if !defined(AFX_DLGBINDINGBOSELF48UNIT_H__AE925D83_04F3_4A06_8CAD_0E42D4DE21AF__INCLUDED_)
#define AFX_DLGBINDINGBOSELF48UNIT_H__AE925D83_04F3_4A06_8CAD_0E42D4DE21AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingBoseLf48Unit.h : header file
//

#include "DlgBindingUnitProp.h"
#include "BindingUIElement.h"
class CBindingBoseLf48Unit;




/////////////////////////////////////////////////////////////////////////////
// CDlgBindingBoseLf48Unit dialog

class CDlgBindingBoseLf48Unit : public CDlgBindingUnitProp
{
protected:
	CBindingBoseLf48Unit *pBindingBoseLf48Unit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

// Construction
public:
	CDlgBindingBoseLf48Unit(CWnd* pParent = NULL);   // standard constructor
	//CBindingBoseLf48Unit *pBindingBoseLf48Unit;


// Dialog Data
	//{{AFX_DATA(CDlgBindingBoseLf48Unit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingBoseLf48Unit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);
	virtual void OnSpin3DataChanged(int iStep);
	void UpdateControls(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingBoseLf48Unit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGBOSELF48UNIT_H__AE925D83_04F3_4A06_8CAD_0E42D4DE21AF__INCLUDED_)
