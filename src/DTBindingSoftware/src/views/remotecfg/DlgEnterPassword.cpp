// DlgEnterPassword.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgEnterPassword.h"


// CDlgEnterPassword dialog

IMPLEMENT_DYNAMIC(CDlgEnterPassword, CDialog)

CDlgEnterPassword::CDlgEnterPassword(CWnd* pParent, const CString& currentPassword)
	: CDialog(CDlgEnterPassword::IDD, pParent) , m_strPassword(currentPassword) { }

CDlgEnterPassword::~CDlgEnterPassword() { }

void CDlgEnterPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Control(pDX, IDOK, m_BtnOk);
}


BEGIN_MESSAGE_MAP(CDlgEnterPassword, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEnterPassword::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgEnterPassword message handlers
void CDlgEnterPassword::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (!m_strPassword.IsEmpty()) {
		CDialog::OnOK();
	}
}

BOOL CDlgEnterPassword::OnInitDialog() {
	return this->CDialog::OnInitDialog();
}