#if !defined(AFX_PROPERTYPAGENODEBACKUP_H__0A8AEC5A_5293_47BE_9D3F_822CE5197F37__INCLUDED_)
#define AFX_PROPERTYPAGENODEBACKUP_H__0A8AEC5A_5293_47BE_9D3F_822CE5197F37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeBackup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBackup dialog

#include "PropertyPageNode.h"

//class CCanNode;
class CTXBrowsePnpMessages;
class CMemoryDumpInfo;

class CPropertyPageNodeBackup : public CPropertyPageNode
{
protected:

	//CCanNode *pSelNode;
	CMemoryDumpInfo* const m_ptrMemoryDumpInfo;
	CString szDynName,szStaticName,szBmpName,szAudioBmpName;


	DECLARE_DYNCREATE(CPropertyPageNodeBackup)

// Construction
public:
	virtual void UpdateRunTimeControls();

	CPropertyPageNodeBackup();
	CPropertyPageNodeBackup(CCanNode *pNode);
	~CPropertyPageNodeBackup();

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeBackup)
	enum { IDD = IDD_DIALOG_NODEBACKUP };
	CEdit	m_EditMaxErrors;
	CStatic	m_StaticInfo;
	CComboBox	m_CmbMemoryType;
	CEdit	m_EditBitmaps;
	CEdit	m_EditAudioBmp;
	CEdit	m_EditStatic;
	CEdit	m_EditDynamic;
	CButton	m_CheckStatic;
	CButton	m_CheckDynamic;
	CButton	m_CheckAudioBitmaps;
	CButton	m_CheckAllBitmaps;
	CButton	m_BtnUpload;
	CButton	m_BtnDownload;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeBackup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RequestRuntimeStatussen(void);
	BOOL fEnableMemoryDump;
	void UpdateStaticControls(void);
	BOOL fInitControls;
	void UpdateControls(void);
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeBackup)
	afx_msg void OnButtonDownload();
	afx_msg void OnButtonUpload();
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseupComboMemory();
	afx_msg void OnButtonSetmaxerrors();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODEBACKUP_H__0A8AEC5A_5293_47BE_9D3F_822CE5197F37__INCLUDED_)
