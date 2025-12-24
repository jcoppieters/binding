// DlgMultimedia.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "DlgMultimedia.h"
#include "MainFrm.h"
#include "App/AppCfg/AppCfgMM/ConfigMultiMediaInfo.h"
#include "App/AppCfg/AppCfgMM/ConfigMultiMediafile.h"
#include "MyWindowMessages.h"

#include "utils/win32/ShellCommands.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
// ------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMultimedia dialog


CDlgMultimedia::CDlgMultimedia(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMultimedia::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMultimedia)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgMultimedia::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMultimedia)
	DDX_Control(pDX, IDC_LIST_INFO, m_ListErrors);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMultimedia, CDialog)
	//{{AFX_MSG_MAP(CDlgMultimedia)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SENDTOALL, OnButtonSendtoall)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMultimedia message handlers

void CDlgMultimedia::OnButtonLoad() 
{

#ifdef SKIP

	if (CConfigMultiMediaInfo::GetInstance() != NULL) 
	{
		CConfigMultiMediaInfo::GetInstance()->Load();
	}
#endif

	UpdateControls();
}

void CDlgMultimedia::OnButtonSendtoall() 
{		

#ifdef SKIP
	
	if (CConfigMultiMediaInfo::GetInstance() != NULL) 
	{
		CConfigMultiMediaInfo::GetInstance()->SendToAllNodes(TRUE);
				
		// TM20091218: Om compatible te zijn met de nieuwe interface geven
		// we het node address van de master LCD mee.
		CConfigMultiMediaInfo::GetInstance()->Init(TRUE,TMGetDocumentNodes(),0xFC);

		TMGetGlobalHSM()->EvOperationUploadMMConfig();	
	}

#endif

}

BOOL CDlgMultimedia::OnInitDialog() 
{
	CDialog::OnInitDialog();

#ifdef SKIP

	if (CConfigMultiMediaInfo::GetInstance() != NULL)
	{
		CConfigMultiMediaInfo::GetInstance()->Load();
	}

#endif


	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMultimedia::UpdateControls()
{	

#ifdef SKIP

	BOOL fEnableSend;

	if (CConfigMultiMediaInfo::GetInstance() != NULL) 
	{
		if (CConfigMultiMediaInfo::GetInstance()->IsFileLoaded() == TRUE) 
		{
			m_ListErrors.ResetContent();
			m_ListErrors.AddString("File is succesfull loaded");
		
			fEnableSend = TRUE;
		}
		else 
		{
			CConfigMultiMediaFile *pConfigFile = NULL;

			fEnableSend = FALSE;

			m_ListErrors.ResetContent();
			m_ListErrors.InsertString(0,"No file is loaded");
			
			if ((pConfigFile = CConfigMultiMediaInfo::GetInstance()->GetMultiMediaFile()) 
				!= NULL)		
			{
				const int iErrors = pConfigFile->GetNrErrors();
				if (iErrors != 0) 
				{
					CString s;
					m_ListErrors.InsertString(1,"Following errors occured:");
					
					for (int i=0;i<iErrors;i++) 
					{
						s = pConfigFile->GetErrors(i);
						m_ListErrors.InsertString(i+2,s);					
					}				
				}
			}
		}
	}

	CWnd *pButton = NULL;
	if ((pButton = GetDlgItem(IDC_BUTTON_SENDTOALL)) != NULL)
	{
		pButton->EnableWindow(fEnableSend);
	}

	if ((pButton = GetDlgItem(IDC_BUTTON_EDIT)) != NULL)
	{
		pButton->EnableWindow(fEnableSend);
	}

#endif

}

void CDlgMultimedia::OnButtonEdit() 
{
	if (CConfigMultiMediaInfo::GetInstance() != NULL) 
	{
		CString szPathName(CConfigMultiMediaInfo::GetInstance()->GetFullPathName());

		TCommandShell<> cmd(szPathName,"edit");
		cmd.Execute();
	}
}
