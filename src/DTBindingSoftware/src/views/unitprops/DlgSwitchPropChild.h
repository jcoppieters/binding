#if !defined(AFX_CDlgSwitchPropChild_H__CDABDAE7_71DE_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_CDlgSwitchPropChild_H__CDABDAE7_71DE_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSwitchPropChild.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgSwitchPropChild dialog

#include "DlgUnitChildProp.h"
//#include "MyEditCtrl.h"				//READ ONLY EDIT BOX !

#include "uicontrols/controls/MyEditCtrl.h"

#include "App\Appbindings\appbindingproperty\BindingPropertyUnit.h"



class CDlgSwitchPropChild : public CDlgUnitChildProp
{
// Construction
public:
	CDlgSwitchPropChild(CWnd* pParent = NULL);   // standard constructor

	virtual void OnNotifyClose(void);
	void InitControls(CBindingPropertyUnit *ptr);

// Dialog Data
	//{{AFX_DATA(CDlgSwitchPropChild)
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSwitchPropChild)
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

	//void CreateSpin(CEdit *pEdit, UINT EditIDD, CSpinButtonCtrl *pSpin, UINT SpinIDD, CStatic *pStatic, CPoint TopLeft, CString szText);

	afx_msg void OnUpDownPrecisionTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckPrecisionTiming(void);

	// Generated message map functions
	//{{AFX_MSG(CDlgSwitchPropChild)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	CBindingPropertySwitchUnit m_bindingSwitchUnit;
	CBindingPropertySwitchUnit *ptrOriginalSwitchUnit;

	CButton m_BtnCheckTiming;
	CStatic m_StaticTime;
	CMyEditCtrl m_EditBuddyTime;
	CSpinButtonCtrl m_SpinTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSWITCHPROPCHILD_H__CDABDAE7_71DE_11D8_B865_0050BAC412B1__INCLUDED_)
