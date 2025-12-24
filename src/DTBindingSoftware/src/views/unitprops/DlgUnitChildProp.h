#if !defined(AFX_DLGUNITCHILDPROP_H__5CE194E1_71B0_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_DLGUNITCHILDPROP_H__5CE194E1_71B0_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgUnitChildProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgUnitChildProp dialog

//#include "BindingConversionToNodesProperty.h"

class CBindingPropertyUnit;


#define BUTTON_RESTORE_DEFAULTS_OFFSET_X	10
#define BUTTON_RESTORE_DEFAULTS_OFFSET_Y	(-15)

#define DEFAULT_BUTTON_X_SIZE				100
#define DEFAULT_BUTTON_Y_SIZE				25


class CDlgUnitChildProp : public CDialog
{
// Construction
public:
	CDlgUnitChildProp(CWnd* pParent = NULL);   // standard constructor
	void ShowWindow(BOOL fShow);
	
	//void UpdateControls(BYTE bNodeAddress,BYTE bUnitAddress);
	//virtual void OnNotifyClose(void);

	virtual void OnNotifyClose(void) = 0;
	virtual void InitControls(CBindingPropertyUnit *ptr) = 0;

// Dialog Data
	//{{AFX_DATA(CDlgUnitChildProp)
	enum { IDD = IDD_DIALOG_PROP_UNIT_CHILD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUnitChildProp)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
private:
	virtual void CreateDefaultButton(void);

protected:
	BOOL m_fChangesMade;
	CRect MyWindowRect;
	BYTE bNodeAddress;
	BYTE bUnitAddress;

protected:
	CButton m_BtnRestoreDefaults;	
	virtual void InitStaticControls(void);
	virtual void InitControlProperties(void);
	virtual void RestoreDefaults(void) = 0;
	BOOL ViewModeExtended() const;

	// Generated message map functions
	//{{AFX_MSG(CDlgUnitChildProp)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnButtonDefaults(void);
	//afx_msg void OnButtonAccept(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUNITCHILDPROP_H__5CE194E1_71B0_11D8_B865_0050BAC412B1__INCLUDED_)
