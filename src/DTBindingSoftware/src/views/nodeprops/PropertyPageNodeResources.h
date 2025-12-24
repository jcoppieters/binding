#if !defined(AFX_PROPERTYPAGENODERESOURCES_H__B71B4781_A991_11D8_B865_0050BAC412B1__INCLUDED_)
#define AFX_PROPERTYPAGENODERESOURCES_H__B71B4781_A991_11D8_B865_0050BAC412B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeResources.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeResources dialog
//class CTXNodeMessage;

#include "PropertyPageNode.h"
class CCanNode;

class CPropertyPageNodeResources : public CPropertyPageNode
{
	DECLARE_DYNCREATE(CPropertyPageNodeResources)
	//CCanNode *pSelNode;
	BOOL fInitControls;

// Construction
public:
	CPropertyPageNodeResources();
	CPropertyPageNodeResources(CCanNode *pNode);
	~CPropertyPageNodeResources();

	virtual void UpdateRunTimeControls();

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeResources)
	enum { IDD = IDD_DIALOG_NODE_RESOURCES };
	CListBox	m_ListNodeType;
	CListBox	m_ListErrorsCAN;
	CListBox	m_ListErrorsNetwork;
	CListBox	m_ListResources;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeResources)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeResources)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateStaticControls();
	void RequestRuntimeStatussen(void);

	CString GetSeperator(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODERESOURCES_H__B71B4781_A991_11D8_B865_0050BAC412B1__INCLUDED_)
