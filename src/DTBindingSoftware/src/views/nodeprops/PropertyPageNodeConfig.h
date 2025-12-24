#if !defined(AFX_PROPERTYPAGENODECONFIG_H__BB667AB9_3E49_44ED_A150_8D0303B209F4__INCLUDED_)
#define AFX_PROPERTYPAGENODECONFIG_H__BB667AB9_3E49_44ED_A150_8D0303B209F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageNodeConfig.h : header file
//


/////////////////////////////////////////////////////////////////////////////
class CCanNode;
#include "PropertyPageNode.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeConfig dialog

class CPropertyPageNodeConfig : public CPropertyPageNode
{
	DECLARE_DYNCREATE(CPropertyPageNodeConfig)

protected:
	BOOL m_fInitControls;
	BOOL m_fSupportedByModemVersion;
	BOOL m_foundConfigurator;			// Configurator tool aanwezig ?	
	BOOL m_foundMMConfig;				// Multimedia config aanwezig ?
	int m_stringIndex;

	enum { MAX_CHECKBUTTONS = 12 };			/** @attention !!!!!! */
	CButton* m_pButtons[ MAX_CHECKBUTTONS ];

// Construction
public:
	CPropertyPageNodeConfig();
	CPropertyPageNodeConfig(CCanNode *pNode);
	~CPropertyPageNodeConfig();
	virtual void UpdateRunTimeControls();
	void AddMessage(const char* s);

// Dialog Data
	//{{AFX_DATA(CPropertyPageNodeConfig)
	enum { IDD = IDD_DIALOG_NODE_CONFIG };
	CButton m_CheckConfigRoomAudio;
	CButton m_CheckConfigGateway;
	CButton m_CheckSmapAVRoom;
	CButton	m_CheckSmapMyhome;
	CButton	m_CheckSmapMain;
	CButton	m_CheckBitmaps;
	CButton	m_CheckSchedule;
	CButton	m_CheckIRTX;
	CButton	m_CheckIRRX;
	CButton	m_CheckMultimedia;
	CButton	m_CheckUnits;
	CButton	m_CheckMoods;
	CListBox m_listStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageNodeConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertyPageNodeConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonSend();
	afx_msg void OnCheckConfigMoods();
	afx_msg void OnCheckConfigUnits();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonSendToAll();
	afx_msg void OnCheckConfigMultimedia();
	afx_msg void OnCheckConfigIrrx();
	afx_msg void OnCheckConfigIrtx();
	afx_msg void OnCheckConfigSchedule();
	afx_msg void OnCheckConfigBitmaps();
	afx_msg void OnCheckConfigSmapMain();
	afx_msg void OnCheckConfigSmapMyhome();
	afx_msg void OnCheckConfigSmapAVRoom();
	afx_msg void OnCheckConfigGateway();
	afx_msg void OnBnClickedCheckConfigRoomctrlAudio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void clearMessages( );
	void StartUpdateTimer( );
	void StopUpdateTimer( );
	void UpdateStaticControls( );
	void onCheckConfigButton(CWnd* pSelected);		
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGENODECONFIG_H__BB667AB9_3E49_44ED_A150_8D0303B209F4__INCLUDED_)
