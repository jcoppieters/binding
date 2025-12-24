#if !defined(AFX_DLGDUOSWITCHCHILDPROP_H__761D66E1_733C_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGDUOSWITCHCHILDPROP_H__761D66E1_733C_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDuoSwitchChildProp.h : header file
//

#include "DlgUnitChildProp.h"
//#include "MyEditCtrl.h"			//READ ONLY EDIT BOX !

#include "uicontrols/controls/MyEditCtrl.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgDuoSwitchChildProp dialog

class CDlgDuoSwitchChildProp : public CDlgUnitChildProp
{
// Construction
public:
	CDlgDuoSwitchChildProp(CWnd* pParent = NULL);   // standard constructor
	virtual void OnNotifyClose(void);
	void InitControls(CBindingPropertyUnit *ptr);

// Dialog Data
	//{{AFX_DATA(CDlgDuoSwitchChildProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDuoSwitchChildProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	virtual void InitStaticControls(void);
	virtual void InitControlProperties(void);	
	virtual void RestoreDefaults(void);

	void InitControlsToDefaults(void);
	void UpdateControls(void);		
	void CheckBorders(BYTE* pValue,BOOL fDirection,BYTE bMinValue,BYTE bMaxValue);

	afx_msg void OnUpDownStopTimer(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpDownSwitchTimer(NMHDR* pNMHDR, LRESULT* pResult);

	// Generated message map functions
	//{{AFX_MSG(CDlgDuoSwitchChildProp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBindingPropertyDuoSwitchUnit m_bindingDuoSwitchUnit;
	CBindingPropertyDuoSwitchUnit *ptrOriginalDuoSwitchUnit;

	CStatic m_StaticStopTime,m_StaticSwitchTime;
	CMyEditCtrl m_EditStopTime,m_EditSwitchTime;
	CSpinButtonCtrl m_SpinStopTime,m_SpinSwitchTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDUOSWITCHCHILDPROP_H__761D66E1_733C_11D8_B865_0050BAC412B1__INCLUDED_)
