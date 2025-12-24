// DlgMessageTransfer.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgMessageTransfer.h"
// -------------------------------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
// -------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CDlgMessageTransfer* CDlgMessageTransfer::m_pDialog = 0;
/////////////////////////////////////////////////////////////////////////////
void CDlgMessageTransfer::Show()
{
	if (m_pDialog == 0) 
	{
		m_pDialog = new CDlgMessageTransfer();
	
		if (m_pDialog->Create() == TRUE)
		{
			m_pDialog->ShowWindow(TRUE);
		}
	}
	else 
	{
		m_pDialog->SetActiveWindow();
	}
}

void CDlgMessageTransfer::Destroy()
{
	if (m_pDialog != 0) 
	{
		m_pDialog->DestroyWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
// CDlgMessageTransfer dialog


CDlgMessageTransfer::CDlgMessageTransfer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMessageTransfer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMessageTransfer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgMessageTransfer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessageTransfer)
	DDX_Control(pDX, IDC_EDIT_UNITADDRESS, m_EditUnitAddress);
	DDX_Control(pDX, IDC_EDIT_NODEADDRESS, m_EditNodeAddress);
	DDX_Control(pDX, IDC_EDIT_MESSAGEDATA, m_EditMessageData);
	DDX_Control(pDX, IDC_EDIT_MESSAGECODE, m_EditMessageCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessageTransfer, CDialog)
	//{{AFX_MSG_MAP(CDlgMessageTransfer)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageTransfer message handlers

void CDlgMessageTransfer::OnOK() 
{
	DestroyWindow();
}

void CDlgMessageTransfer::OnButtonSend() 
{	
	int NodeAddress,UnitAddress,MsgCode;
	BOOL fValid = FALSE;

	if (TMIsConnected())
	{
		if (((GetData(&m_EditNodeAddress,&NodeAddress)) == TRUE) &&
			((GetData(&m_EditUnitAddress,&UnitAddress)) == TRUE) &&
			((GetData(&m_EditMessageCode,&MsgCode)) == TRUE)) 
		{	
			CString szMessageData;
			const int nLength = GetData(&m_EditMessageData,szMessageData);
			if (nLength == 0)
			{
				fValid = (BOOL) TMGetCommunicationApp()->
					MessageTransfer((BYTE) NodeAddress,
								    (BYTE) UnitAddress,
									(BYTE) MsgCode);
			}
			else
			{			
				fValid = (BOOL) TMGetCommunicationApp()->
					MessageTransfer((BYTE) NodeAddress,
								    (BYTE) UnitAddress,
									(BYTE) MsgCode,
									(BYTE) nLength,
									szMessageData);			
			}
		}
	}
	if (fValid == FALSE)
	{
		CString szMessage("Unable to dispatch message to the CAN network");
		AfxMessageBox(szMessage);
	}
}

BOOL CDlgMessageTransfer::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgMessageTransfer::GetData(CEdit *pControl, int *pValue)
{
	ASSERT(pControl);
	ASSERT(pValue);

	CString szTemp;
	BOOL fValid = FALSE;

	pControl->GetWindowText(szTemp);
	if (szTemp.IsEmpty() == FALSE)
	{
		// Oppassen: Moeten "%02x" meegeven aan sscanf( ) omdat de volledige string aan
		// elkaar hangt en we telkens 2 Hex digits nodig hebben...

		if (sscanf((const char*) szTemp,"%02x",pValue) == 1)		// CHECKED_SSCANF_OK
		{
			fValid = TRUE;
		}
	}
	return fValid;
}

int CDlgMessageTransfer::GetData(CEdit *pControl, CString& szMessageData)
{
	ASSERT(pControl);

	CString szTemp;
	int nLength = 0;

	pControl->GetWindowText(szTemp);
	if (szTemp.IsEmpty() == FALSE)
	{		
		const int strLen = szTemp.GetLength();
		szMessageData = szTemp;
		nLength = (strLen/2);
	}
	else
	{
		szMessageData.Empty();
	}
	return nLength;
}

void CDlgMessageTransfer::PostNcDestroy() 
{
	delete this;
	m_pDialog = 0;
}

void CDlgMessageTransfer::OnClose() 
{
	DestroyWindow();
}
