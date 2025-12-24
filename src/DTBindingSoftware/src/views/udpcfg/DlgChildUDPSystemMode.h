#pragma once

#include "DlgChildUDPBase.h"
#include "afxwin.h"

// CDlgUDPSystemMode dialog

class CDlgChildUDPSystemMode : public CDlgChildUDPBase
{
	DECLARE_DYNAMIC(CDlgChildUDPSystemMode)

public:
	CDlgChildUDPSystemMode(CWnd* pParent,CDlgUDPConfigCtx& refCtx);   // standard constructor
	virtual ~CDlgChildUDPSystemMode();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_SYSTEMMODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnCbnSelchangeComboSystemMode();
	virtual void initData(AppUDPDevice& refDevice);		
	virtual void initStaticControls(AppUDPDevice& refDevice);
	virtual void updateDynamicControls(AppUDPDevice& refDevice);
	virtual void onTimerExpiry(AppUDPDevice& refDevice, UINT_PTR nIDEvent);
	virtual BOOL dataChanged( );
	virtual void onActive(BOOL flag);
	virtual const CString& getStrDescription( ) const;
	BOOL busy();
	bool sendConfig(AppUDPDevice& refDevice, AppUDPServerCfgWrapper::ErrorResponseCode_t* pResponseCode);

private:
	typedef enum {
		STATE_INIT = 0,
		STATE_IDLE,
		STATE_BUSY	
	} State_t;

	State_t m_state;				// de toestand.
	CComboBox m_cmbSystemMode;		// Combobox basic/extended.
	CButton m_btnConfigure;			// Configure btn.
	int m_proxySystemMode;			// Proxy
	int m_retryCounter;				// retry counnter doorsturen van de cfg.
};
