#if !defined(AFX_PROPERTYPAGENODEBITMAPS_H__9C11828C_2237_4C20_AF2E_7D4456B5CDC7__INCLUDED_)
#define AFX_PROPERTYPAGENODEBITMAPS_H__9C11828C_2237_4C20_AF2E_7D4456B5CDC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeBitmaps.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmaps dialog


// ---------------------------------
#include "uicontrols/controls/MyButton.h"

#include "MemoryBitmapInfo.h"
#include "PropertyPageNode.h"

//class CCanNode;
// ----------------------------------

class CPropertyPageNodeBitmaps : public CPropertyPageNode
{
	DECLARE_DYNCREATE(CPropertyPageNodeBitmaps)

// Construction
public:
	CPropertyPageNodeBitmaps(CCanNode *pNode);
	CPropertyPageNodeBitmaps();
	~CPropertyPageNodeBitmaps();

	virtual void UpdateRunTimeControls() {};

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeBitmaps)
	enum { IDD = IDD_DIALOG_NODE_BITMAPS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeBitmaps)
	public:
	virtual void OnSetFont(CFont* pFont);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeBitmaps)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	/***************************************************************//**
	 * m_fRetrievePathName
	 * @brief	PathName opvragen a/d globale settings.
	 *			boolean die bepaalt of het path moet worden opvraagd
	 *			a/d globale settings.
	 *
	 * TM 20081224	
	 ******************************************************************/
	bool m_fRetrievePathName;		

	CMemoryBitmapInfo* const m_ptrMemoryBitmapInfo;

	CButton			m_BtnClear,m_BtnUpload,m_BtnUploadAll;

	CFont			*m_pDefaultFont;

	LONG OnMyButtonClicked(WPARAM wParam,LPARAM lParam);
	afx_msg void OnButtonUpload();
	afx_msg void OnButtonClearmemory();
	afx_msg void OnButtonUploadAll();

	virtual void CreateButtons(void) {ASSERT(0);};
	virtual void UpdateButtons(void) {ASSERT(0);};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODEBITMAPS_H__9C11828C_2237_4C20_AF2E_7D4456B5CDC7__INCLUDED_)
