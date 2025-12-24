#if !defined(AFX_DLGBINDINGVIDEOMUXPROP_H__6FF158D7_3F15_4C61_AD1B_7884D8E8B876__INCLUDED_)
#define AFX_DLGBINDINGVIDEOMUXPROP_H__6FF158D7_3F15_4C61_AD1B_7884D8E8B876__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingVideomuxProp.h : header file
//


#include "DlgBindingUnitProp.h"
#include "bindingUiElement.h"
class CBindingVideomuxUnit;


/////////////////////////////////////////////////////////////////////////////
// CDlgBindingVideomuxProp dialog

class CDlgBindingVideomuxProp : public CDlgBindingUnitProp
{
protected:
	CBindingVideomuxUnit *pBindingVideomuxUnit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

// Construction
public:
	CDlgBindingVideomuxProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingVideomuxProp)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingVideomuxProp)
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
	//{{AFX_MSG(CDlgBindingVideomuxProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGVIDEOMUXPROP_H__6FF158D7_3F15_4C61_AD1B_7884D8E8B876__INCLUDED_)
