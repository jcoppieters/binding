#if !defined(AFX_PROPERTYPAGENODEBINDINGS_H__910595C9_A980_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_PROPERTYPAGENODEBINDINGS_H__910595C9_A980_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeBindings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBindings dialog

#include "PropertyPageNode.h"

//class CCanNode;
class CBindingFile;


class CPropertyPageNodeBindings : public CPropertyPageNode
{
	DECLARE_DYNCREATE(CPropertyPageNodeBindings)

	CBindingFile* const m_ptrBindingFile;
	BOOL fInitControls;

// Construction
public:
	CPropertyPageNodeBindings();
	CPropertyPageNodeBindings(CCanNode *pNode);
	~CPropertyPageNodeBindings();

	void UpdateRunTimeControls(void);

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeBindings)
	enum { IDD = IDD_DIALOG_NODE_BINDINGS };
	CButton	m_BtnWrite;
	CButton	m_BtnRead;
	CButton	m_BtnErase;
	CListBox	m_ListBindings;
	CString	m_szBindingFileInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeBindings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeBindings)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnErase();
	afx_msg void OnBtnRead();
	afx_msg void OnBtnWrite();
	afx_msg void OnBtnLoad();
	afx_msg void OnBtnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateStaticControls(void);
	void RequestRuntimeStatussen(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODEBINDINGS_H__910595C9_A980_11D8_B865_0050BAC412B1__INCLUDED_)
