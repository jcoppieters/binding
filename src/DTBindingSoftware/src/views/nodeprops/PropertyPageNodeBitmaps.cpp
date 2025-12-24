// PropertyPageNodeBitmaps.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeBitmaps.h"

#include "MainFrm.h"
#include "MyWindowMessages.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;		//TM 20081224
// -----------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmaps property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeBitmaps, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyPageNodeBitmaps::CPropertyPageNodeBitmaps() : 
CPropertyPageNode(CPropertyPageNodeBitmaps::IDD),
m_ptrMemoryBitmapInfo(0),
m_fRetrievePathName( true )
{
	//{{AFX_DATA_INIT(CPropertyPageNodeBitmaps)
	//}}AFX_DATA_INIT

	ASSERT(m_pSelNode);
	ASSERT(m_pDefaultFont == 0);
}

CPropertyPageNodeBitmaps::CPropertyPageNodeBitmaps(CCanNode *pNode) 
: CPropertyPageNode(CPropertyPageNodeBitmaps::IDD,pNode),
m_ptrMemoryBitmapInfo(CMemoryBitmapInfo::GetInstance()),
m_pDefaultFont(0),
m_fRetrievePathName( true )
{
	ASSERT(m_pSelNode);
	ASSERT(m_pDefaultFont == 0);

	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		if (m_ptrMemoryBitmapInfo->IsLocked() == FALSE) 
		{
			m_ptrMemoryBitmapInfo->FreeAllocatedMemory();
																							
			if (m_ptrMemoryBitmapInfo->AllocateMemoryForNode(TMGetRuntimeNodes(),
															 m_pSelNode) 
															 == FALSE) 
			{			
					// Generate Error ...		
				ASSERT(0);
			}				
		}
	}
}

CPropertyPageNodeBitmaps::~CPropertyPageNodeBitmaps()
{
}

void CPropertyPageNodeBitmaps::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);				// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_DATA_MAP(CPropertyPageNodeBitmaps)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeBitmaps, CPropertyPage)
	//{{AFX_MSG_MAP(CPropertyPageNodeBitmaps)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, OnButtonUpload)
	ON_BN_CLICKED(IDC_BUTTON_CLEARMEMORY, OnButtonClearmemory)
	ON_MESSAGE(WM_MYBUTTON_CLICKED,OnMyButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_UPLOADALL,OnButtonUploadAll)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBitmaps message handlers

BOOL CPropertyPageNodeBitmaps::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();						// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	
	m_BtnClear.Create("Clear Bitmap memory",
					    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					   CRect(0,0,50,50),
					   this,
					   IDC_BUTTON_CLEARMEMORY);
	
	m_BtnUpload.Create("Upload Bitmaps to LCD",
					    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					   CRect(0,0,50,50),
					   this,
					   IDC_BUTTON_UPLOAD);	

	m_BtnUploadAll.Create("Upload Bitmaps to All LCDs",
					    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					   CRect(0,0,50,50),
					   this,
					   IDC_BUTTON_UPLOADALL);	
	
	// Handeled by Derived Classes
	CreateButtons();	
	UpdateButtons();

	BOOL fEnable = TRUE;
	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		if (m_ptrMemoryBitmapInfo->IsLocked()) 
			fEnable = FALSE;
	}

	m_BtnClear.EnableWindow(fEnable);
	m_BtnUpload.EnableWindow(fEnable);
	m_BtnUploadAll.EnableWindow(fEnable);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyPageNodeBitmaps::OnButtonClearmemory() 
{
	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		m_ptrMemoryBitmapInfo->ClearAllBitmaps();	
		UpdateButtons();	// Handled by Derived Classes
	}
}

void CPropertyPageNodeBitmaps::OnButtonUpload() 
{
	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		if (m_ptrMemoryBitmapInfo->GetNrOfStoredBitmaps() > 0)
		{
			if (m_ptrMemoryBitmapInfo->SetTask(MEMORYBITMAPINFO_TASK_UPLOAD)) 
			{		
				TMGetGlobalHSM()->EvOperationUploadAllBitmaps();				
			}
		}
	}
}
void CPropertyPageNodeBitmaps::OnButtonUploadAll()
{
	if (m_ptrMemoryBitmapInfo != NULL) 
	{
		if (m_ptrMemoryBitmapInfo->GetNrOfStoredBitmaps() > 0)
		{
			if(m_ptrMemoryBitmapInfo->SetTask(MEMORYBITMAPINFO_TASK_UPLOADTOALL)) 
			{		
				TMGetGlobalHSM()->EvOperationUploadAllBitmaps();
			}
		}
	}
}
	
LONG CPropertyPageNodeBitmaps::OnMyButtonClicked(WPARAM wParam,LPARAM lParam)
{
	if (m_ptrMemoryBitmapInfo != NULL) 
	{	
		CString szDirConfig(TMGetFileSettings()->GetDirConfig());

		CFileDialog CFileOpen(TRUE,
					_T("bmp"),
					_T("*.bmp"),
					OFN_HIDEREADONLY|OFN_PATHMUSTEXIST,
					_T("Bitmap (*.bmp) | *.bmp | All Files (*.*) | *.* ||"),
					this);

	
		// TM 20081224
		if (m_fRetrievePathName == true)
		{		
			if (!szDirConfig.IsEmpty()) 
			{
				CFileOpen.m_ofn.lpstrInitialDir = szDirConfig;					
			}				
		}

		if (CFileOpen.DoModal() == IDOK) 
		{
			/* Solved BUG-0125 */
			CString szFileName(CFileOpen.GetPathName());
			if (!szFileName.IsEmpty()) 
			{			
				// Try to Load Bitmap file ...						
				const int iButtonIndex = (int) wParam;
				if (!(m_ptrMemoryBitmapInfo->StoreBitmap(szFileName,iButtonIndex))) 
				{						
						// Generate-Errors ...
				}		

				m_fRetrievePathName = false;
			}
		}	
		// Update Button Controls ...
		UpdateButtons();
	}
	return TRUE;
}


void CPropertyPageNodeBitmaps::OnSetFont(CFont* pFont) 
{
	m_pDefaultFont = pFont;
	CPropertyPage::OnSetFont(pFont);
}
