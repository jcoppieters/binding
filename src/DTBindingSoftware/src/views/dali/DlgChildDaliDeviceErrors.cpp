// DlgChildDaliDeviceErrors.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgChildDaliDeviceErrors.h"
#include "appdlgdaliworker.h"

// CDlgChildDaliDeviceErrors dialog
using namespace DUOTECNO::DALI;

IMPLEMENT_DYNAMIC(CDlgChildDaliDeviceErrors, CDialog)

CDlgChildDaliDeviceErrors::CDlgChildDaliDeviceErrors(CAppDlgDaliWorker& refWorker,CWnd* pParent /*=NULL*/)
	: CDlgChildDali(refWorker,CDlgChildDaliDeviceErrors::IDD, pParent), m_strTextInfo( )
{
	this->setInitDone(false);
}

CDlgChildDaliDeviceErrors::~CDlgChildDaliDeviceErrors()
{
}

void CDlgChildDaliDeviceErrors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ICON, m_staticIcon);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_staticTextInfo);
}


BEGIN_MESSAGE_MAP(CDlgChildDaliDeviceErrors, CDialog)
	ON_MESSAGE(WM_UPDATE_CONTROLS, OnUpdateControls)
END_MESSAGE_MAP()


// CDlgChildDaliDeviceErrors message handlers
void CDlgChildDaliDeviceErrors::init(DUOTECNO::DALI::CAppDaliDevice* pDevice) {	
	this->setDevice(pDevice);	
	this->initModel(pDevice);
	this->PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgChildDaliDeviceErrors::OnParentPropertyPageActive(DUOTECNO::DALI::CAppDaliDevice* pDevice,bool activeDialog) {	
	if (activeDialog) {
		
		// TODO...

	}
}

LRESULT CDlgChildDaliDeviceErrors::OnUpdateControls(WPARAM wParam, LPARAM lParam) {
	if (!this->getInitDone()) {
		this->initStaticIcon( );
		this->setInitDone(true);
	}
	m_staticTextInfo.SetWindowText(this->m_strTextInfo);
	return TRUE;
}

void CDlgChildDaliDeviceErrors::initModel(DUOTECNO::DALI::CAppDaliDevice* pDevice) {

	if (NULL == pDevice) {
		m_strTextInfo.Empty( );
	}
	else {		
		const DaliDeviceType_t type = pDevice->getType( );		

		m_strTextInfo = CString("A problem has been detected with this DALI device.\r\n\r\n");

		m_strTextInfo += CString("When in online mode, you can locate the device using the 'Identify Device' option.\r\n" 
							     "This button can be found at the top of the dialog box (see device info).\r\n");

		if (DALI_DEVICE_TYPE_CONTROLGEAR == type) {		
			m_strTextInfo += 
				CString("When the 'Identify Device' option is enabled, the device will blink on and off at 1-second intervals.\r\n\r\n\r\n");		
		}
		else {				
			m_strTextInfo += CString("When the 'Identify Device' option is enabled, the device will indicate its location using a control LED or a buzzer. " 
									 "The method of identification depends on the device manufacturer.\r\n\r\n");
			m_strTextInfo += CString("A more effective way to locate devices is by operating them and monitoring their activity "
									 "on the 'Locate Devices' page.\r\n\r\n\r\n");
		}	

		m_strTextInfo += CString("On the next tab, you can optionally modify the addresses or remove the device from the system.\r\n\r\n");

		m_strTextInfo += CString("Once you believe the issues are resolved, perform a new 'Search devices' or 'Extend installation' to confirm "
								 "that all problems have been eliminated. These options can be found under the 'Network' tab.\r\n\r\n"                  
							     "Make sure all issues are fully resolved before making any changes to the DALI Duotecno Bridge configuration.");
	}
}


void CDlgChildDaliDeviceErrors::initStaticIcon( ) {
	HICON hIconLarge = (HICON)LoadImage(
        NULL,
        MAKEINTRESOURCE(IDI_ERROR),
        IMAGE_ICON,
        48, 48,
        LR_SHARED
    );
	if (NULL != hIconLarge) {
		this->m_staticIcon.SetIcon(hIconLarge);
	}
}