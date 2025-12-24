#pragma once

#include "DlgChildUDPBase.h"
#include "afxwin.h"

// CDlgChildUDPHomeserver dialog

class CDlgChildUDPHomeserver : public CDlgChildUDPBase
{
	DECLARE_DYNAMIC(CDlgChildUDPHomeserver)

public:
	typedef enum {
		MODE_HOMESERVER = 0,
		MODE_API
	} Mode_t;

	CDlgChildUDPHomeserver(CWnd* pParent,CDlgUDPConfigCtx& refCtx,Mode_t mode);   // standard constructor
	virtual ~CDlgChildUDPHomeserver();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_HOMESERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
	virtual void initData(AppUDPDevice& refDevice);					// Algemene data.
	virtual void initStaticControls(AppUDPDevice& refDevice);		// In de init - overschrijven door afgeleide klassen.
	virtual void updateDynamicControls(AppUDPDevice& refDevice);	// Wordt assync aangeroepen.
	virtual void onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent);
	virtual BOOL dataChanged( );
	virtual void onActive(BOOL flag);
	virtual const CString& getStrDescription( ) const;
	BOOL busy();
	afx_msg void OnBnClickedButtonConfigure();
	bool sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);

	// Controls.
	int getCtrlIPPort( );
	BOOL getCtrlPassword(CString* pStr);

private:
	enum { 
		DEFAULT_IPPORT = 5001,
		MAX_LENGTH_PASSWORD = 16			// 15 karakters + NULL char.
	};
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY	
	} State_t;
	Mode_t m_mode;
	State_t m_state;
	CStatic m_staticGroup;
	CStatic m_staticIPPort;
	CEdit m_editIPPort;
	CEdit m_editPassword;
	CButton m_btnConfigure;
	CString m_proxyPassword;		// Proxy voor UDP device data.
	int m_proxyPort;				// Proxy voor UDP device data.
	int m_retryCounter;
};
