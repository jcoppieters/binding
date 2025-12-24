// DlgCommTracer.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgCommTracer.h"

#include "MainFrm.h"
#include "MyWindowMessages.h"
//#include "ComApplication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
CDlgCommTracer* CDlgCommTracer::m_pDlgTracer = NULL;
/////////////////////////////////////////////////////////////////////////////
// CDlgCommTracer dialog

CDlgCommTracer::CDlgCommTracer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCommTracer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCommTracer)
	//}}AFX_DATA_INIT

	//m_pParent = pParent;
	m_nID = CDlgCommTracer::IDD;
}


void CDlgCommTracer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCommTracer)
	DDX_Control(pDX, IDC_LISTBOX, m_ListBoxCommunication);
	DDX_Control(pDX, IDC_LISTBOX2, m_ListBoxErrors);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgCommTracer, CDialog)
	//{{AFX_MSG_MAP(CDlgCommTracer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////

void CDlgCommTracer::Show(void) {
	if (m_pDlgTracer == NULL)  {
		m_pDlgTracer = new CDlgCommTracer();	
		if (m_pDlgTracer->Create() == TRUE) {
			m_pDlgTracer->ShowWindow(TRUE);
		}
	}
	else {
		m_pDlgTracer->SetActiveWindow();
	}
}

void CDlgCommTracer::AddMsgTracerComm(const CString& s) {
	if (m_pDlgTracer != NULL) {
		static const int LISTBOX_TOP_INDEX = 10;
		static const int LISTBOX_MAX_MESSAGES = 10000;
		m_pDlgTracer->AddMessage(&m_pDlgTracer->m_ListBoxCommunication,s, LISTBOX_MAX_MESSAGES, LISTBOX_TOP_INDEX);
	}
}

void CDlgCommTracer::AddMsgTracerError(const CString& s) {
	if (m_pDlgTracer != NULL) {
		static const int LISTBOX_TOP_INDEX = 5;
		static const int LISTBOX_MAX_MESSAGES = 100;
		m_pDlgTracer->AddMessage(&m_pDlgTracer->m_ListBoxErrors,s, LISTBOX_MAX_MESSAGES, LISTBOX_TOP_INDEX);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCommTracer message handlers

void CDlgCommTracer::OnOK() 
{
	DestroyWindow();	
}

void CDlgCommTracer::OnCancel() 
{
	DestroyWindow();
}

void CDlgCommTracer::PostNcDestroy() 
{
	delete this;
	m_pDlgTracer = NULL;
}

// Sedert 0x0800: Extra optimalisatie.
void CDlgCommTracer::AddMessage(CListBox* pListBox,const CString& szData,int maxMessages,int topIndex)
{   
	const CTime t(CTime::GetCurrentTime());

	CString szMessage;
	szMessage.Format("%s%s",t.Format("%H:%M:%S "),szData);   

	int count = pListBox->GetCount();
	if (count >= maxMessages) {
		pListBox->ResetContent();
		count=0;
	}

	// LB_ERRSPACE: Niet genoeg plaats in de listbox om string toe te voegen...
	const int resValue = pListBox->InsertString(count,szMessage);
	if (LB_ERRSPACE==resValue) {		
		pListBox->ResetContent();   
	}
	else if (LB_ERR != resValue) {
		count++;	
		pListBox->SetTopIndex(count-topIndex);	
	}
}
