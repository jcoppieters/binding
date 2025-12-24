#if !defined(AFX_PROPERTYPAGENODEBITMAPSBASIC_H__2EA52208_43CA_4294_8209_8F9C8582725B__INCLUDED_)
#define AFX_PROPERTYPAGENODEBITMAPSBASIC_H__2EA52208_43CA_4294_8209_8F9C8582725B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeBitmapsBasic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsBasic dialog

#include "PropertyPageNodeBitmaps.h"

//------------------------------------------------------------

class CPropertyPageNodeBitmapsBasic : public CPropertyPageNodeBitmaps 
{
	DECLARE_DYNCREATE(CPropertyPageNodeBitmapsBasic)

// Construction
public:
	CPropertyPageNodeBitmapsBasic(CCanNode *pNode);
	CPropertyPageNodeBitmapsBasic();
	~CPropertyPageNodeBitmapsBasic();

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeBitmapsBasic)
	enum { IDD = IDD_DIALOG_NODE_BITMAPS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeBitmapsBasic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeBitmapsBasic)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CMyButton	m_Button[BITMAP_BASIC_V64_MAX_BITMAPS];
	CButton		m_StaticMoods;

	virtual void CreateButtons(void);
	virtual void UpdateButtons(void);

	//virtual LONG OnMyButtonClicked(WPARAM wParam,LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODEBITMAPSBASIC_H__2EA52208_43CA_4294_8209_8F9C8582725B__INCLUDED_)
