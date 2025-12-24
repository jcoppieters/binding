#pragma once

/////////////////////////////////////////////////////////////////////////////
// Oppassen: Dit bestand bevat definitie van 
// - Basisklasse CPropertyPageNodeConfig_BaseV2
// - Afgeleide klasse: CPropertyPageNodeConfigV2
// - Afgeleide klasse: CPropertyPageNodeScreenmapV2
/////////////////////////////////////////////////////////////////////////////

class CCanNode;
#include "PropertyPageNode.h"
#include "afxwin.h"

#include "app\appcfg\appcfgalgview\configalgview.h"
#include "app\appcfg\appcfgalgview\ConfigAlgViewfactory.h"


// CPropertyPageNodeConfigV2 dialog

class CPropertyPageNodeConfig_BaseV2 : public CPropertyPageNode
{
	DECLARE_DYNAMIC(CPropertyPageNodeConfig_BaseV2)

	class CViewDisplay : public CConfigAlgView::IViewDisplay
	{
	public:
		CViewDisplay(CPropertyPageNodeConfig_BaseV2& parent) :
		  refParent(parent) { };
		virtual void AddMessage(const char* s)
		{
			refParent.AddMessage(s);
		}
	private:
		CPropertyPageNodeConfig_BaseV2& refParent;
	};

public:
	CPropertyPageNodeConfig_BaseV2();
	CPropertyPageNodeConfig_BaseV2(CCanNode* pNode,int maxAlgoritmes);
	virtual ~CPropertyPageNodeConfig_BaseV2();
	virtual BOOL OnKillActive( );
	void AddMessage(const char* s);

// Dialog Data
	enum { IDD = IDD_DIALOG_NODE_CONFIG_V2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	// void initButtons(void);
	virtual void allocAlgoritmes(CCanNode* pNode) = 0;
	void deallocAlgoritmes(void);
	void attachAlg2Buttons(void);

	virtual void UpdateRunTimeControls();
	void UpdateStaticControls();
	void StartUpdateTimer( );
	void StopUpdateTimer( );	
	void clearMessages( );
	BOOL FindSelectedAlgoritme(CConfigAlgView** p);

	afx_msg void OnCheckButtonClicked(UINT nID);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonReceive();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonSendToAll();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

protected:
	enum 
	{ 
		MAX_CHECKBUTTONS = 12,
	};
	
	CConfigAlgView** m_pAlgView; //[ MAX_ALGORITMES];
	CViewDisplay m_ViewDisplay;
	BOOL m_fInitDialog;
	BOOL m_foundConfigurator;
	BOOL m_fSupportedByModemVersion;
#ifdef SKIP
	BOOL m_fChanges;
#endif
	CListBox m_listStatus;
	int m_stringIndex;

	static const int CHECKBUTTON_CONTROL_IDS[MAX_CHECKBUTTONS];	

private:
	const int m_MaxAlgoritmes;
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheckConfig11();
	afx_msg void OnBnClickedCheckOverrideHttp();
};


// CPropertyPageNodeConfigV2

class CPropertyPageNodeConfigV2 : public CPropertyPageNodeConfig_BaseV2
{
public:
	CPropertyPageNodeConfigV2();
	CPropertyPageNodeConfigV2(CCanNode* pNode);
	virtual ~CPropertyPageNodeConfigV2();

protected:
	virtual void allocAlgoritmes(CCanNode* pNode);

private:
	enum { MAX_ALGORITMES   = 17 };	
	static const CConfigAlgViewID CONFIG_ALGORITME_IDS[MAX_ALGORITMES];	
};


// CPropertyPageNodeScreenmapV2

class CPropertyPageNodeScreenmapV2 : public CPropertyPageNodeConfig_BaseV2
{
public:
	CPropertyPageNodeScreenmapV2();
	CPropertyPageNodeScreenmapV2(CCanNode* pNode);
	virtual ~CPropertyPageNodeScreenmapV2();

protected:
	virtual void allocAlgoritmes(CCanNode* pNode);

private:
	enum { MAX_ALGORITMES   = 4 };
	static const CConfigAlgViewID CONFIG_ALGORITME_IDS[MAX_ALGORITMES];	
};