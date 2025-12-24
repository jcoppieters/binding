#if !defined(AFX_DLGBINDINGIRTXUNIT_H__86C38127_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGIRTXUNIT_H__86C38127_87CB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingIRTXUnit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "DlgBindingUnitProp.h"
#include "bindingUIElement.h"
class CBindingIRTXUnit;

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingIRTXUnit dialog
class CDlgBindingIRTXUnit : public CDlgBindingUnitProp
{
protected:
	CBindingIRTXUnit *pBindingIRTXUnit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

	/* 
	 * TM 20120830; bepaalt of het configIRTX bestand moet herladen worden.
	 * De eerste maal een naam opgevraagd wordt via deze dialoog is dit nodig,
	 * nadien niet meer
	 */
	BOOL m_fReloadIRTXFile ;	

public:
	CDlgBindingIRTXUnit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingIRTXUnit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingIRTXUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	void UpdateControls(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingIRTXUnit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGIRTXUNIT_H__86C38127_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
