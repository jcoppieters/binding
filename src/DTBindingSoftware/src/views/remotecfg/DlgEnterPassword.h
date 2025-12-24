#pragma once

// CDlgEnterPassword dialog
class CDlgEnterPassword : public CDialog
{
	DECLARE_DYNAMIC(CDlgEnterPassword)

public:
	CDlgEnterPassword(CWnd* pParent,const CString& currentPassword);   // standard constructor
	virtual ~CDlgEnterPassword();

	const CString& password( ) const {
		return m_strPassword;
	}

// Dialog Data
	enum { IDD = IDD_DIALOG_PASSWORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

private:
	CString m_strPassword;
	CButton m_BtnOk;
};
