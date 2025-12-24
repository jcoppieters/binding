#if !defined(AFX_PROPERTYPAGENODEBITMAPSHC4_H__1088A898_97E0_4E37_A122_3F2AB92A2014__INCLUDED_)
#define AFX_PROPERTYPAGENODEBITMAPSHC4_H__1088A898_97E0_4E37_A122_3F2AB92A2014__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeBitmapsHC4.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmapsHC4 dialog

#include "PropertyPageNodeBitmaps.h"

///////////////////////////////////////////////////////////
#define MAX_NR_MOOD_BUTTONS				17
#define MAX_NR_MAINSCREEN_AUDIO			11
#define MAX_NR_DESTINATIONS_AUDIO		16
#define MAX_NR_SOURCES_AUDIO			8
#define	MAX_NR_EXTRAIR					5

//#define MAX_NR_

#define MAINSCREEN_HC4MOODS_MOOD0		0
#define MAINSCREEN_HC4MOODS_MOOD16		16

///////////////////////////////////////////////////////////

class CPropertyPageNodeBitmapsHC4 : public CPropertyPageNodeBitmaps
{
	DECLARE_DYNCREATE(CPropertyPageNodeBitmapsHC4)

// Construction
public:
	CPropertyPageNodeBitmapsHC4();
	CPropertyPageNodeBitmapsHC4(CCanNode *pNode);
	~CPropertyPageNodeBitmapsHC4();

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeBitmapsHC4)
	enum { IDD = IDD_DIALOG_NODE_BITMAPS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeBitmapsHC4)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	//afx_msg void OnBtnClicked();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeBitmapsHC4)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CMyButton				m_MoodButtons[MAX_NR_MOOD_BUTTONS];
	CMyButton				m_MainScreenActionButtons[MAX_NR_MAINSCREEN_AUDIO];
	CMyButton				m_AudioDestButtons[MAX_NR_DESTINATIONS_AUDIO];
	CMyButton				m_AudioSources[MAX_NR_SOURCES_AUDIO];
	CMyButton				m_AudioExtraIR[MAX_NR_EXTRAIR];


	CButton		m_StaticMoods,m_StaticDest,m_StaticSources,m_StaticExtra;

	virtual void CreateButtons(void);
	virtual void UpdateButtons(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODEBITMAPSHC4_H__1088A898_97E0_4E37_A122_3F2AB92A2014__INCLUDED_)
