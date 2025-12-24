// PropertyPageNewAppPage1.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNewAppPage1.h"
#include "iappdlgtasks.h"


// CPropertyPageNewAppPage1 dialog

IMPLEMENT_DYNAMIC(CPropertyPageNewAppPage1, CPropertyPage)

CPropertyPageNewAppPage1::CPropertyPageNewAppPage1(CPropertySheet* parent,CPropertyPageNewAppContext& refCtx)
	: CPropertyPageNewApp(parent, CPropertyPageNewAppPage1::IDD, refCtx)
	, m_radioProjectType(0)
{
}

CPropertyPageNewAppPage1::~CPropertyPageNewAppPage1()
{
}

void CPropertyPageNewAppPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NEW, m_radioProjectType);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_btnOpenProject);
	DDX_Control(pDX, IDC_EDIT_PROJECTNAME, m_editProjectName);
	DDX_Control(pDX, IDC_GROUP_OPENPROJECT, m_staticGroupOpen);
	DDX_Control(pDX, IDC_STATIC_INFO_FOOTER, m_staticInfoFooter);
}


BEGIN_MESSAGE_MAP(CPropertyPageNewAppPage1, CPropertyPage)
	ON_MESSAGE(WM_UPDATE_CONTROLS,&CPropertyPageNewAppPage1::OnUpdateControls)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CPropertyPageNewAppPage1::OnBnClickedButtonOpen)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_NEW, IDC_RADIO_LOAD, OnRadioButtonsType)
END_MESSAGE_MAP()


// CPropertyPageNewAppPage1 message handlers
void CPropertyPageNewAppPage1::UpdateCommunicationStatus() 
{
	// absorbe...
}


BOOL CPropertyPageNewAppPage1::OnInitDialog()
{
	CPropertyPageNewApp::OnInitDialog();

	{
		CPropertyPageNewAppContext& refCtx = this->getCtx();	
		m_radioProjectType = (int)  refCtx.getProjectType();

		UpdateData(FALSE);
	}

	PostMessage(WM_UPDATE_CONTROLS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


LONG CPropertyPageNewAppPage1::OnUpdateControls(WPARAM wParam,LPARAM lParam) {

	BOOL enableBtnNext = FALSE;
	const BOOL enableBtnPrevious = FALSE;
	CString strMsgInfo;
	CPropertyPageNewAppContext& refCtx = this->getCtx();
	CPropertyPageNewAppContext::ProjectType_t type = refCtx.getProjectType();
	switch(type) {
		case CPropertyPageNewAppContext::PROJECT_TYPE_EXISTING: {
			m_btnOpenProject.ShowWindow(TRUE);			
			m_staticGroupOpen.ShowWindow(TRUE);
			
			if (refCtx.getProjectLoaded()) {
				const CString projectName = refCtx.getProjectName();
				m_editProjectName.ShowWindow(TRUE);
				m_editProjectName.SetWindowText(projectName);
				m_btnOpenProject.EnableWindow(FALSE);	
				this->enableWindow(this,IDC_RADIO_NEW,FALSE);
				enableBtnNext = TRUE;
				strMsgInfo = CString("Press 'Next' to continue.");
			}
			else {
				this->enableWindow(this,IDC_RADIO_NEW,TRUE);
				m_editProjectName.ShowWindow(FALSE);
				m_btnOpenProject.EnableWindow(TRUE);
				strMsgInfo = CString("Open project to continue.");
			}
		} break;
		case CPropertyPageNewAppContext::PROJECT_TYPE_NEW: {
			m_btnOpenProject.ShowWindow(FALSE);																
			m_staticGroupOpen.ShowWindow(FALSE);
			m_editProjectName.ShowWindow(FALSE);
			strMsgInfo = CString("Press 'Next' to continue.");
			enableBtnNext = TRUE;
		} break;
	}

	m_staticInfoFooter.SetWindowText(strMsgInfo);

	DWORD dwButtons = 0;
	if (enableBtnNext) {
		dwButtons += PSWIZB_NEXT;
	}
	if (enableBtnPrevious) {
		dwButtons += PSWIZB_BACK;
	}
	this->setWizardButtons(dwButtons);
	return TRUE;
}

void CPropertyPageNewAppPage1::OnBnClickedButtonOpen()
{
	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	CString szProjectName;
	BOOL fLoaded =  refCtx.getHost().loadDocumentFile(&szProjectName);
	refCtx.setProjectLoaded(fLoaded);
	if (fLoaded) {
		refCtx.setProjectName(szProjectName);
		this->updateContext();
	}	

	// Update GUI.
	PostMessage(WM_UPDATE_CONTROLS);	
}

// Handlers controls.
void CPropertyPageNewAppPage1::OnRadioButtonsType(UINT nID) {

	UpdateData(TRUE);		// Update control data --> m_radioProjectType

	// Update context.
	CPropertyPageNewAppContext& refCtx = this->getCtx();	
	if (0 == m_radioProjectType) {
		refCtx.setProjectType(CPropertyPageNewAppContext::PROJECT_TYPE_NEW);
	}
	else {
		refCtx.setProjectType(CPropertyPageNewAppContext::PROJECT_TYPE_EXISTING);
	}

	// Update GUI.
	PostMessage(WM_UPDATE_CONTROLS);	
}