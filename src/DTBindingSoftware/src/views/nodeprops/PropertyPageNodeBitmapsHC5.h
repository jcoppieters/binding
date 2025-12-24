#if !defined(AFX_PROPERTYPAGENODEBITMAPSHC5_H__AE2F9A1D_E0B9_4C34_A8A7_F832D5C9F42E__INCLUDED_)
#define AFX_PROPERTYPAGENODEBITMAPSHC5_H__AE2F9A1D_E0B9_4C34_A8A7_F832D5C9F42E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeBitmapsHC5.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsHC5 dialog

#include "PropertyPageNodeBitmaps.h"


// --------------------------------------------------------------------------

#define MAX_NR_DESTINATIONS_AUDIO		16
#define MAX_NR_SOURCES_AUDIO			8
#define	MAX_NR_EXTRAIR					5


// --------------------------------------------------------------------------

class CPropertyPageNodeBitmapsHC5 : public CPropertyPageNodeBitmaps
{
	DECLARE_DYNCREATE(CPropertyPageNodeBitmapsHC5)

// Construction
public:
	CPropertyPageNodeBitmapsHC5(CCanNode *pNode);
	CPropertyPageNodeBitmapsHC5();
	~CPropertyPageNodeBitmapsHC5();

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeBitmapsHC5)
	enum { IDD = IDD_DIALOG_NODE_BITMAPS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeBitmapsHC5)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeBitmapsHC5)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CMyButton				m_AudioDestButtons[MAX_NR_DESTINATIONS_AUDIO];
	CMyButton				m_AudioSources[MAX_NR_SOURCES_AUDIO];
	CMyButton				m_AudioExtraIR[MAX_NR_EXTRAIR];

	CButton		m_StaticDest,m_StaticSources,m_StaticExtra;


	virtual void CreateButtons(void);
	virtual void UpdateButtons(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODEBITMAPSHC5_H__AE2F9A1D_E0B9_4C34_A8A7_F832D5C9F42E__INCLUDED_)
