#if !defined(AFX_PROPERTYPAGENODESCREENMAP_H__75ACEA20_D9BF_4BB6_9D48_EC1C911D9E47__INCLUDED_)
#define AFX_PROPERTYPAGENODESCREENMAP_H__75ACEA20_D9BF_4BB6_9D48_EC1C911D9E47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeScreenmap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeScreenmap dialog


#include "NodeLcdScreenmaps.h"
#include "PropertyPageNode.h"


class CCanNode;
class CNodeDatabase;



class CEditSubClass : public CEdit
{
protected:
	int iLineNR,iMaxLineNr;
	int iPageNR,iMaxPageNr;
	int iButtonNR,iMaxButtonNr;

	int iButtonsPerPage;

public:
   CEditSubClass();   // standard constructor

protected:
   afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
   afx_msg void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point) ;
   DECLARE_MESSAGE_MAP()

public:
   void UpdateParentControls(void);

   void SetButtonsPerPage(int i)	{ iButtonsPerPage = i; };
};

/**
 *
 */

class CPropertyPageNodeScreenmap : public CPropertyPageNode
{
	DECLARE_DYNCREATE(CPropertyPageNodeScreenmap)

	CNodeDatabase* const m_pNodeDatabase;
	CNodeLcdScreenmaps* const m_pNodeLcdScreenmaps;

	CString m_szPathName;
	IScreenmap* m_ptrScreenmap;	

protected:

	enum enum_screenmaps 
	{
		enum_screenmap_dimmers = 0,
		enum_screenmap_switch = 1,
		enum_screenmap_moods = 2,
		enum_screenmap_sens = 3,
		enum_screenmap_duoswitch = 4,
		enum_screenmap_audio = 5	
		
	}screenmapChoice;

// Construction
public:
	CPropertyPageNodeScreenmap();
	CPropertyPageNodeScreenmap(CCanNode *pNode,CNodeDatabase *pDatabase,CString string);
	~CPropertyPageNodeScreenmap();

	virtual void UpdateRunTimeControls(void) { };

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeScreenmap)
	enum { IDD = IDD_DIALOG_NODE_SCREENMAP };
	CStatic	m_StaticStart;
	CEdit	m_editStart;
	CComboBox	m_ComboCopyTo;
	CButton	m_BtnCopy;
	CStatic	m_StaticPageNr;
	CStatic	m_StaticLineNr;
	CStatic	m_StaticButtonNr;
	int		m_RadioDimmer;
	CString	m_StringContent;
	//}}AFX_DATA

	CEditSubClass	m_EditBox;

	void UpdateLineNr(int i,int max);
	void UpdatePageNr(int i,int max);
	void UpdateButtonNr(int i,int max);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeScreenmap)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateCopyScreenmapControls();
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeScreenmap)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonInit();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonUpdate();
	afx_msg void OnButtonCopy();
	afx_msg void OnButtonUpdateAll();
	afx_msg void OnKillfocusEditStart();
	afx_msg void OnKillfocusEditScrmapbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnRadioButtonClicked(UINT nCmdID);

	BOOL UpdateScreenmapChoice(void);
	BOOL StoreScreenmapChoice(void);
	BOOL CopyScreenmaps(CCanNode *pNode);
	BOOL UpdateScreenmapDefaultPageNR( );

private:
	BOOL saveScreenmaps( );
	BOOL loadScreenmaps( );
	BOOL saveifModified( );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODESCREENMAP_H__75ACEA20_D9BF_4BB6_9D48_EC1C911D9E47__INCLUDED_)
