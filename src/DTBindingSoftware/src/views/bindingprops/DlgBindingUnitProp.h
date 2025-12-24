#if !defined(AFX_DLGBINDINGUNITPROP_H__534A5501_870B_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGBINDINGUNITPROP_H__534A5501_870B_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBindingUnitProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgBindingUnitProp dialog
class CBindingUnit;
class CMainFrame;
#include "uicontrols/controls/MyComboBox.h"
#include "uicontrols/controls/MyEditCtrl.h"
#include "uicontrols/controls/UnitButton.h"
#include "App\Appbindings\appbindingproperty\BindingPropertyStrings.h"

class CDlgBindingUnitProp : public CDialog
{
protected:		
	static CDlgBindingUnitProp *m_pBindingDlgUnit;
	static CBindingUnit *m_pBindingUnit;
	int m_nID;

public:
	static BOOL Show(CBindingUnit *pBindingUnit,CPoint TopLeft,const CString& strLabelInfo);
	static BOOL Destroy();
	static BOOL OnUnitStatusChanged(BYTE bNodeAddress,BYTE bUnitAddress);

protected:
	void InitControls(CBindingUnit *pUnit,const CString& strLabelInfo);
	void MoveWindow(CPoint topleft);

	void EnableControls(BOOL fSpin1=FALSE,BOOL fSpin2=FALSE,BOOL fSpin3=FALSE,BOOL fSpin4=FALSE);
	void UpdateRuntimeProperties(void);
	 
	BOOL Create() { 
		return CDialog::Create(m_nID,AfxGetMainWnd());
	}	


	void EnableSpinCtrl(CMyEditCtrl* pEditCtrl,CSpinButtonCtrl* pSpin,BOOL enable);

public:
	CDlgBindingUnitProp(CWnd* pParent = NULL);   // standard constructor

	CBindingUnit *GetBindingUnit(void) 
	{
		return m_pBindingUnit;
	};


// Dialog Data
	//{{AFX_DATA(CDlgBindingUnitProp)
	enum { IDD = IDD_DIALOG_PROP_BINDINGUNIT };
	CUnitButton	m_BtnControl;
	CSpinButtonCtrl	m_Spin4;
	CSpinButtonCtrl	m_Spin3;
	CSpinButtonCtrl	m_Spin2;
	CSpinButtonCtrl	m_Spin1;
	CMyEditCtrl	m_EditSpin4;
	CMyEditCtrl	m_EditSpin3;
	CMyEditCtrl	m_EditSpin2;
	CMyEditCtrl	m_EditSpin1;
	CMyComboBox	m_CmbMethod;
	CString	m_szAddress;
	CString	m_szUnitName;
	CString m_labelTextInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBindingUnitProp)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	 virtual void InitControls(void){};
	 virtual void UpdateControls(void){};
	 virtual void OnCmbBoxDataChanged(void) {};
	 virtual void OnSpin1DataChanged(int iStep) {};
	 virtual void OnSpin2DataChanged(int iStep) {};
	 virtual void OnSpin3DataChanged(int iStep) {};
	 virtual void OnSpin4DataChanged(int iStep) {};
	 	 
	 BOOL ViewModeExtended() const;
	 BOOL ViewModeNormal() const;
	// Generated message map functions
	//{{AFX_MSG(CDlgBindingUnitProp)
	afx_msg void OnSelendokComboMethod();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBINDINGUNITPROP_H__534A5501_870B_11D8_B865_0050BAC412B1__INCLUDED_)
