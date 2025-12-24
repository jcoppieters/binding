// PropertyPageNodeBindings.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeBindings.h"

#include "CanNode.h"
#include "BindingFile1.h"
#include "BindingFileFSM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBindings property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeBindings, CPropertyPage)			// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyPageNodeBindings::CPropertyPageNodeBindings() : 
CPropertyPageNode(CPropertyPageNodeBindings::IDD),
fInitControls(FALSE),
m_ptrBindingFile(0)
{
	//{{AFX_DATA_INIT(CPropertyPageNodeBindings)
	m_szBindingFileInfo = _T("");
	//}}AFX_DATA_INIT

	ASSERT( m_pSelNode );
	ASSERT( m_ptrBindingFile );
	ASSERT( fInitControls == FALSE);
}


//////////////////////////////////////////////////////////////////////////
CPropertyPageNodeBindings::CPropertyPageNodeBindings(CCanNode *pNode) : 
CPropertyPageNode(CPropertyPageNodeBindings::IDD,pNode),
m_ptrBindingFile(pNode->GetBindingFilePtr()),
fInitControls(FALSE)
{	
	ASSERT( m_pSelNode );
	ASSERT( m_ptrBindingFile );
	ASSERT( fInitControls == FALSE );
}


CPropertyPageNodeBindings::~CPropertyPageNodeBindings()
{
}

void CPropertyPageNodeBindings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeBindings)
	DDX_Control(pDX, IDC_BTN_WRITE, m_BtnWrite);
	DDX_Control(pDX, IDC_BTN_READ, m_BtnRead);
	DDX_Control(pDX, IDC_BTN_ERASE, m_BtnErase);
	DDX_Control(pDX, IDC_LIST_BINDINGS, m_ListBindings);
	DDX_Text(pDX, IDC_STATIC_BINDINGFILE_INFO, m_szBindingFileInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeBindings, CPropertyPage)			// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_MSG_MAP(CPropertyPageNodeBindings)
	ON_BN_CLICKED(IDC_BTN_ERASE, OnBtnErase)
	ON_BN_CLICKED(IDC_BTN_READ, OnBtnRead)
	ON_BN_CLICKED(IDC_BTN_WRITE, OnBtnWrite)
	ON_BN_CLICKED(IDC_BTN_LOAD, OnBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBindings message handlers

BOOL CPropertyPageNodeBindings::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();									// CHANGES_MVS_2008 : Afleiden van CPropertyPage

	fInitControls = TRUE;	
	UpdateStaticControls();
	UpdateRunTimeControls();
	RequestRuntimeStatussen();

//#ifndef _DEBUG
	
	m_BtnWrite.EnableWindow(FALSE);
	m_BtnRead.EnableWindow(FALSE);
	m_BtnErase.EnableWindow(FALSE);

//#endif	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPropertyPageNodeBindings::UpdateRunTimeControls(void)
{
	
	if (fInitControls == TRUE)
	{
		ASSERT(m_pSelNode);

		m_szBindingFileInfo.Format(_T("Nr of BindingEntries : %d , XOR Checksum = %02x"),
								   m_pSelNode->getNodeBindingsNrEntries(),
								   m_pSelNode->getNodeBindingsXORChecksum());

		UpdateData(FALSE);
	}
}

void CPropertyPageNodeBindings::UpdateStaticControls(void)
{
	ASSERT(m_ptrBindingFile);
		
	CString string;
	for (int i=0,k=m_ptrBindingFile->Count();
		 i<k;
		 i++) 
	{
		CString szBindingString(m_ptrBindingFile->GetString(i));			
				
		string.Format(_T("%03d : %s"),i+1,szBindingString);

		m_ListBindings.AddString(string);
	}
}

void CPropertyPageNodeBindings::RequestRuntimeStatussen(void)
{
}

/**
 * Debug : Buttons Disabled !!!!
 */
void CPropertyPageNodeBindings::OnBtnErase() 
{
	ASSERT(m_ptrBindingFile);
	
	if (m_ptrBindingFile != 0)
	{
		m_ptrBindingFile->GetFSM()->StartEraseBindings();
	}
}

/**
 * Debug : Buttons Disabled !!!!
 */
void CPropertyPageNodeBindings::OnBtnRead() 
{
	ASSERT(m_ptrBindingFile);

	if (m_ptrBindingFile != 0)
	{
		m_ptrBindingFile->Clear();
		m_ptrBindingFile->GetFSM()->StartReadBindings();
	}
}

/**
 * Debug : Buttons Disabled !!!!
 */
void CPropertyPageNodeBindings::OnBtnWrite() 
{
	ASSERT(m_ptrBindingFile);
	m_ptrBindingFile->GetFSM()->StartWriteBindings();
}


void CPropertyPageNodeBindings::OnBtnLoad() 
{
	ASSERT(m_ptrBindingFile);
	m_ListBindings.ResetContent();
	
	if (m_ptrBindingFile != 0)
	{
		m_ptrBindingFile->Clear();	
		m_ptrBindingFile->Load();
	
		UpdateStaticControls();
	}
}

void CPropertyPageNodeBindings::OnBtnSave() 
{
	ASSERT(m_ptrBindingFile);

	if (m_ptrBindingFile != 0)
		m_ptrBindingFile->Save();
}
