#pragma once

#include "PropertyPageNode.h"
#include "afxwin.h"

// CPropertyPageNodeSmapLite dialog

class CPropertyPageNodeSmapLite : public CPropertyPageNode
{
	DECLARE_DYNAMIC(CPropertyPageNodeSmapLite)

public:
	CPropertyPageNodeSmapLite();
	CPropertyPageNodeSmapLite(CCanNode *pNode);	
	virtual ~CPropertyPageNodeSmapLite();

	typedef struct
	{		
		CString m_szExtSensName;
		BYTE m_bExtSensNodeAddress;
		BYTE m_bExtSensUnitAddress;
		BOOL m_fExtSensUseExternal;
		BOOL m_fChanged;

	} CfgData_t;

	typedef enum
	{
		// Alle controls disabled.
		DISPLAY_MODE_EXTSENS_NOT_SUPPORTED = 0,

		// Enkel de checkbox enabled. (+send)
		DISPLAY_MODE_EXTSENS_USE_INTERNAL_SENS,

		// Alle controls.
		DISPLAY_MODE_EXTSENS_USE_EXTERNAL_SENS,

	} DisplayMode_t;

	enum 
	{ 	
		WM_UPDATE_CONTROLS	= (WM_USER + 150),

		PARAM_UPDATE_CONTROLS = 1,
		PARAM_UPDATE_DATA	  = 0,

		WM_UPDATE_STATIC_CONTROLS = (WM_UPDATE_CONTROLS+1)
	};

	afx_msg void OnBnClickedBtnExternalSens();
	afx_msg void OnBnClickedCheckExternalSens();
	afx_msg void OnSelectExtSensor(void);
	virtual BOOL OnInitDialog();
	virtual void UpdateRunTimeControls() { /* ABsorbe */ };
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	LONG OnUpdateControls(WPARAM wParam,LPARAM lParam);
	LONG OnUpdateStaticControls(WPARAM wParam,LPARAM lParam);
	void AddMessage(const char* s);
	void clearMessage(void);

// Dialog Data
	enum { IDD = IDD_DIALOG_NODE_SMAP_LITE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void UpdateStaticControls();

	// Combo box bevat alle sensors - enkel de sensors van deze node niet.
	BOOL initComboBoxEntries();
	BOOL updateComboxBoxEntries();
	BOOL updateCheckBox();
	BOOL getComboBoxData();
	BOOL CheckSaveModified();
	BOOL saveModified();
	BOOL sendCfgData();

	void updateControlsEnabled(void);
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

private:
	CButton m_btnExtSens;
	CButton m_btnCheckExtSens;
	CComboBox m_cmbExtSens;
	CStatic m_groupExtSens;
	CListBox m_listStatus;
	CfgData_t m_data;
	DisplayMode_t m_displayMode;
	int m_stringIndex;

	static void initData(CfgData_t* pData);
};
