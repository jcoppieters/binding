// DlgCommCanLog.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgCommCanLog.h"

/*****************************************************************/
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalSettings;
/*****************************************************************/

/////////////////////////////////////////////////////////////////////////////
CDlgCommCanLog* CDlgCommCanLog::m_pDlgTracer = NULL;
/////////////////////////////////////////////////////////////////////////////

// CDlgCommCanLog dialog

IMPLEMENT_DYNAMIC(CDlgCommCanLog, CDialog)

CDlgCommCanLog::CDlgCommCanLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCommCanLog::IDD, pParent), m_fileLoggingEnable(false)
{
	m_nID = CDlgCommCanLog::IDD;
}

CDlgCommCanLog::~CDlgCommCanLog()
{
}

void CDlgCommCanLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MESSAGES, m_ListBox);
	DDX_Control(pDX, IDC_CHECK1, m_btnCheckLog);
}


BEGIN_MESSAGE_MAP(CDlgCommCanLog, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgCommCanLog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDlgCommCanLog::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDlgCommCanLog::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgCommCanLog::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON_ADDLINE, &CDlgCommCanLog::OnBnClickedButtonAddline)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDlgCommCanLog::OnBnClickedButtonClear)
	ON_MESSAGE(WM_UPDATE_CONTROLS,OnUpdateControls)
END_MESSAGE_MAP()


// CDlgCommCanLog message handlers
void CDlgCommCanLog::Show(void)
{
	if (m_pDlgTracer == NULL) {
		m_pDlgTracer = new CDlgCommCanLog();	
		if (m_pDlgTracer->Create() == TRUE) {
			m_pDlgTracer->ShowWindow(TRUE);
		}
	}
	else {
		m_pDlgTracer->SetActiveWindow();
	}
}

void CDlgCommCanLog::Update(void) {
	if (m_pDlgTracer != NULL) {		
		m_pDlgTracer->PostMessage(WM_UPDATE_CONTROLS);
	}
}

void CDlgCommCanLog::Add(const CString& s)
{
	if (m_pDlgTracer != NULL) {
		m_pDlgTracer->AddMessage(s);
	}
}
void CDlgCommCanLog::OnBnClickedOk()
{
	TMGetCommunicationApp()->SendModemCanLoggerOnOff(FALSE);
	fileLoggingStop();
	DestroyWindow();
}

void CDlgCommCanLog::OnCancel()
{
	TMGetCommunicationApp()->SendModemCanLoggerOnOff(FALSE);
	fileLoggingStop();
	DestroyWindow();
}

void CDlgCommCanLog::PostNcDestroy() 
{
	delete this;
	m_pDlgTracer = NULL;
}

void CDlgCommCanLog::helper_AddMessage(const CString& szData) {

	int count = m_ListBox.GetCount();
	if (count >= 3000) {

		// save to file.
		fileLoggingAppendData();
		
		m_ListBox.ResetContent();
		count=0;
	}

	const int resValue = m_ListBox.InsertString(count,szData);
	if (LB_ERRSPACE==resValue)
	{
		// Niet genoeg plaats in de listbox om string toe te voegen...
		m_ListBox.ResetContent();   
	}
	else if (LB_ERR != resValue)
	{
		count++;	
		m_ListBox.SetTopIndex(count-10);	
	}
}

void CDlgCommCanLog::AddMessage(const CString& szData)
{   
	const CTime t(CTime::GetCurrentTime());

	CString szMessage;
	szMessage.Format("%s%s",t.Format("%d %b, %H:%M:%S "),szData);   

	helper_AddMessage(szMessage);
}
void CDlgCommCanLog::OnBnClickedButtonStart()
{
	TMGetCommunicationApp()->SendModemCanLoggerOnOff(TRUE);
}

void CDlgCommCanLog::OnBnClickedButtonStop()
{
	TMGetCommunicationApp()->SendModemCanLoggerOnOff(FALSE);
}

void CDlgCommCanLog::OnBnClickedCheck1()
{
	if (BST_CHECKED == m_btnCheckLog.GetCheck()) {

		bool overWrite = false;
		if (IDYES == MessageBox("Overwrite file on disk?", NULL, MB_YESNO | MB_ICONQUESTION)) {
			overWrite = true;
		}		
		fileLoggingStart(overWrite);
		m_fileLoggingEnable = true;
	}
	else {
		fileLoggingStop();
		m_fileLoggingEnable = false;
	}
	PostMessage(WM_UPDATE_CONTROLS);
}

void CDlgCommCanLog::OnBnClickedButtonAddline()
{
	const CString msg("---------------------------------------------");
	this->helper_AddMessage(msg);
}

void CDlgCommCanLog::OnBnClickedButtonClear()
{
	// save to file.
	fileLoggingAppendData();

	// Wissen van de content.
	m_ListBox.ResetContent();
}

BOOL CDlgCommCanLog::OnInitDialog()
{
	CDialog::OnInitDialog();

	TMGetCommunicationApp()->SendModemCanLoggerOnOff(TRUE);
	PostMessage(WM_UPDATE_CONTROLS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


static void EnableControl(CWnd* pDialog,UINT ulControlID,BOOL fState) {
	CWnd *pWnd = pDialog->GetDlgItem(ulControlID);
	if (pWnd != NULL) {
		pWnd->EnableWindow(fState);
	}
}

LONG CDlgCommCanLog::OnUpdateControls(WPARAM wParam,LPARAM lParam) {
	const BOOL connected = TMGetCommunicationApp()->GetCommunicationStatus().connected();
	const BOOL loggerBusy = TMGetCommunicationApp()->GetCommunicationStatus().canLoggerEnabled();
	EnableControl(this,IDC_BUTTON_START,!loggerBusy && connected);
	EnableControl(this,IDC_BUTTON_STOP,loggerBusy && connected);
	m_btnCheckLog.SetCheck(m_fileLoggingEnable == true ? BST_CHECKED : BST_UNCHECKED);
	return TRUE;
}

static CString getLogFileName() {
	CString retVal;
	CString szName("canlog.txt");
	CString szSubDir;
	const CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());
	szSubDir.LoadString(IDS_FILENAME_SUBDIR_DATA);
	retVal.Format("%s%s\\%s", szPathName, szSubDir, szName);
	return retVal;
}


void CDlgCommCanLog::fileLoggingStart(bool flagOverwrite) {
	FILE* fd = NULL;
	const CString szTemp = getLogFileName();
	const char* const fileName = (LPCSTR) szTemp;
	if (flagOverwrite) {
		fd = fopen(fileName, "w");
	}
	else {
		fd = fopen(fileName, "a");
	}
	if (NULL != fd) {
		fprintf(fd, "---------------------->Start Logfile<-------------------\n");
		fclose(fd);
		m_fileLoggingEnable = true;
	}
}

void CDlgCommCanLog::fileLoggingStop() {
	if (m_fileLoggingEnable) {
		FILE* fd = NULL;
		const CString szTemp = getLogFileName();
		const char* const fileName = (LPCSTR)szTemp;
		fd = fopen(fileName, "a");
		if (NULL != fd) {
			fileLoggingAppendData(fd);
			fprintf(fd, "--------------------->Stop Logfile<-------------------\n");
			fclose(fd);
		}
		m_fileLoggingEnable = false;
	}
}

void CDlgCommCanLog::fileLoggingAppendData() {
	if (m_fileLoggingEnable) {
		FILE* fd = NULL;
		const CString szTemp = getLogFileName();
		const char* const fileName = (LPCSTR)szTemp;
		fd = fopen(fileName, "a");
		if (NULL != fd) {
			fileLoggingAppendData(fd);		
			fclose(fd);
		}	
	}
}

void CDlgCommCanLog::fileLoggingAppendData(FILE* fp) {
	if (m_fileLoggingEnable) {	
		const int count = m_ListBox.GetCount();
		for (int i = 0; i<count; i++) {
			CString msg;
			m_ListBox.GetText(i, msg);
			fprintf(fp, "%s\n", (LPCSTR) msg);
		} 
	}
}
