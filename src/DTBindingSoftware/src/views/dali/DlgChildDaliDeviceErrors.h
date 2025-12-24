#pragma once

#include "dlgchilddali.h"

// CDlgChildDaliDeviceErrors dialog

class CDlgChildDaliDeviceErrors : public CDlgChildDali
{
	DECLARE_DYNAMIC(CDlgChildDaliDeviceErrors)

public:
	CDlgChildDaliDeviceErrors(CAppDlgDaliWorker& refWorker, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChildDaliDeviceErrors();

	virtual void init(DUOTECNO::DALI::CAppDaliDevice* pDevice);

	virtual void OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog);
	virtual BOOL OnParentPropertyPageLeave() {
		return TRUE;
	}

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD_DALI_DEVICE_ERRORS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	LRESULT OnUpdateControls(WPARAM wParam, LPARAM lParam);

	void initStaticIcon( );
	void initModel(DUOTECNO::DALI::CAppDaliDevice* pDevice);

private:
	CString m_strTextInfo;
	CStatic m_staticIcon;
	CStatic m_staticTextInfo;

	//     CONTROL         "",IDC_STATIC_ICON,"Static",SS_BLACKFRAME,17,20,27,29
    // LTEXT           "Static",IDC_STATIC_MESSAGE,55,19,369,203
};
