#if !defined(AFX_DLGBINDINGSENSUNIT_H__86C38125_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGSENSUNIT_H__86C38125_87CB_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingSensUnit.h : header file
//


#include "DlgBindingUnitProp.h"
#include "BindingUiElement.h"

class CBindingSensUnit;


/////////////////////////////////////////////////////////////////////////////
// CDlgBindingSensUnit dialog

class CDlgBindingSensUnit : public CDlgBindingUnitProp
{
protected:
	CBindingSensUnit *pBindingSensUnit;
	const DUOTECNO::BINDINGS::UI::CStringEntries* m_pStringEntries;

public:
	CDlgBindingSensUnit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBindingSensUnit)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingSensUnit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void InitControls(void);
	virtual void OnCmbBoxDataChanged(void);
	virtual void OnSpin1DataChanged(int iStep);
	virtual void OnSpin2DataChanged(int iStep);
	virtual void OnSpin3DataChanged(int iStep);		// PID
	virtual void OnSpin4DataChanged(int iStep);		// PID
	void UpdateControls(void);

	void SetMode(CEdit& Ctrl);
	void OnSpin_UpdateMode(int iStep);
	void OnSpin_UpdateWorkingMode(int iStep);
	void OnSpin_UpdateFanSpeed(int iStep);

	typedef enum {
		PID_KP = 0, PID_KI, PID_KD
	} PidParams_t;	
	void OnSpin_UpdatePIDParam(int iStep,PidParams_t params);

	// Voor de PID Controller
	int getPIDWorkingMode();
	CString getStrPIDWorkingMode();
	void OnSpin_UpdatePIDWorkingMode(int iStep);
	CString getStrPIDControlUnit(int nodeAddress,int unitAddress);
	void OnSpin_UpdatePIDConrolUnit(int iStep);

	// Generated message map functions
	//{{AFX_MSG(CDlgBindingSensUnit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGSENSUNIT_H__86C38125_87CB_11D8_B865_0050BAC412B1__INCLUDED_)
