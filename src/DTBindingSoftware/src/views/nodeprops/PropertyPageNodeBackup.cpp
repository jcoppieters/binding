// PropertyPageNodeBackup.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeBackup.h"
#include "CanNode.h"
#include "nodegenestructs.h"
#include "TXNodeMessage.h"
#include "MyWindowMessages.h"
#include "MemoryDumpInfo.h"

#include "utils/win32/ShellFunctions.h"


// ------------------------------------
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
// ------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBackup property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeBackup, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyPageNodeBackup::CPropertyPageNodeBackup() : 
CPropertyPageNode(CPropertyPageNodeBackup::IDD),				// CHANGES_MVS_2008 : Afleiden van CPropertyPage
m_ptrMemoryDumpInfo(0)
{
	//{{AFX_DATA_INIT(CPropertyPageNodeBackup)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropertyPageNodeBackup::CPropertyPageNodeBackup(CCanNode *pNode) : 
CPropertyPageNode(CPropertyPageNodeBackup::IDD,pNode),
m_ptrMemoryDumpInfo(CMemoryDumpInfo::GetInstance()),
fInitControls(FALSE)
{		
	ASSERT(m_pSelNode);		
	ASSERT(fInitControls == FALSE);

	if (m_ptrMemoryDumpInfo != 0) 
	{							
		if (m_ptrMemoryDumpInfo->AssignNode(TMGetRuntimeNodes(),m_pSelNode) == FALSE)
		{
			//AfxMessageBox("Failed to assign new node to CMemoryDumpInfo, because memory is locked...");			
		}
			
		fEnableMemoryDump = FALSE;	
	}
}


CPropertyPageNodeBackup::~CPropertyPageNodeBackup()
{
}

void CPropertyPageNodeBackup::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeBackup)
	DDX_Control(pDX, IDC_EDIT_MAXERRORS, m_EditMaxErrors);
	DDX_Control(pDX, IDC_STATIC_INFO, m_StaticInfo);
	DDX_Control(pDX, IDC_COMBO_MEMORY, m_CmbMemoryType);
	DDX_Control(pDX, IDC_EDIT_STATIC_BITMAPS, m_EditBitmaps);
	DDX_Control(pDX, IDC_EDIT_STATIC_AUDIOBMP, m_EditAudioBmp);
	DDX_Control(pDX, IDC_EDIT_STATIC, m_EditStatic);
	DDX_Control(pDX, IDC_EDIT_DYNAMIC, m_EditDynamic);
	DDX_Control(pDX, IDC_CHECK_STATIC, m_CheckStatic);
	DDX_Control(pDX, IDC_CHECK_DYNAMIC, m_CheckDynamic);
	DDX_Control(pDX, IDC_CHECK_AUDIOBITMAPS, m_CheckAudioBitmaps);
	DDX_Control(pDX, IDC_CHECK_ALLBITMAPS, m_CheckAllBitmaps);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, m_BtnUpload);
	DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, m_BtnDownload);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeBackup, CPropertyPage)			// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_MSG_MAP(CPropertyPageNodeBackup)
	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, OnButtonDownload)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, OnButtonUpload)
	ON_CBN_CLOSEUP(IDC_COMBO_MEMORY, OnCloseupComboMemory)
	ON_BN_CLICKED(IDC_BUTTON_SETMAXERRORS, OnButtonSetmaxerrors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeBackup message handlers

void CPropertyPageNodeBackup::OnButtonDownload() 
{
	const BYTE bNodeAddress = m_pSelNode->bLogicalAddress;

	if (m_ptrMemoryDumpInfo != NULL) 
	{		
		if (TMGetFileSettings() != 0) 
		{	
			if (m_ptrMemoryDumpInfo->IsLocked() == FALSE) 
			{
				if ((m_CheckStatic.GetState() & 0x0003) == TRUE) 
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_STATRAM,TRUE);
				else 
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_STATRAM,FALSE);

				if ((m_CheckDynamic.GetState() & 0x0003) == TRUE)
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_DYNRAM,TRUE);
				else 
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_DYNRAM,FALSE);

				if ((m_CheckAllBitmaps.GetState() & 0x0003) == TRUE)
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_ALLBMP,TRUE);
				else 
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_ALLBMP,FALSE);

				if ((m_CheckAudioBitmaps.GetState() & 0x0003) == TRUE)
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_AUDIOBMP,TRUE);
				else 
					m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_AUDIOBMP,FALSE);
						
			
				// TM 20060504: Check Create Directory here !!!!
				if (1)
				{
					CString szSubDir;

					if (szSubDir.LoadString(IDS_FILENAME_SUBDIRBACKUP) == TRUE) 
					{
						CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());

						if ((CreateSubDirectory(szPathName,szSubDir)) == TRUE)
						{
							// Request To download ...
							m_ptrMemoryDumpInfo->SetTask(MEMORYDUMPINFO_TASK_DOWNLOAD,TRUE);
							m_ptrMemoryDumpInfo->SetTask(MEMORYDUMPINFO_TASK_UPLOAD,FALSE);
							m_ptrMemoryDumpInfo->SetTask(MEMORYDUMPINFO_TASK_VERIFY,TRUE);												

							TMGetGlobalHSM()->EvOperationDownloadMemory();
						}				
					}
					else
					{
						CString szError;
						szError.Format(_T("Unable to load string resource : IDS_FILENAME_SUBDIRBACKUP"));
						AfxMessageBox(szError);					
					}				
				}
			}
		}
	}
}


void CPropertyPageNodeBackup::OnButtonUpload() 
{
	unsigned short usFlags = 0;
	CString szInfo;
	BOOL fResult = TRUE;
	BOOL fCorrectVersion= TRUE;
	
	if (m_ptrMemoryDumpInfo != NULL) 
	{	
		if (m_ptrMemoryDumpInfo->IsLocked() == FALSE) 
		{
			const BYTE bNodeAddress = m_pSelNode->bLogicalAddress;
			CMemoryDumpLogFile* const pDumpLogFile = m_ptrMemoryDumpInfo->GetLogFile();

			m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_STATRAM,FALSE);
			m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_DYNRAM,FALSE);
			m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_ALLBMP,FALSE);
			m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_AUDIOBMP,FALSE);

			if ((m_CheckStatic.GetState() & 0x0003) == TRUE) 
			{
				CMemoryDumpFileInfo* const ptrFileInfo = 
					m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_STATRAM);

				if (ptrFileInfo != 0) 
				{
					if (ptrFileInfo->isAvailable()) 
					{
						m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_STATRAM,TRUE);
							
						if (pDumpLogFile!=NULL) 
						{
							if(pDumpLogFile->CheckNode(MEMORYDUMPINFO_TYPE_STATRAM) == FALSE)
							{
								fCorrectVersion = FALSE;
							}
						}
					}
					else fResult = FALSE;
				}
				else fResult = FALSE;
			}

			if ((m_CheckDynamic.GetState() & 0x0003) == TRUE) 
			{
				CMemoryDumpFileInfo* const ptrFileInfo = 
					m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_DYNRAM);
				
				if (ptrFileInfo != 0) 
				{
					if (ptrFileInfo->isAvailable()) 
					{
						m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_DYNRAM,TRUE);
						
						if (pDumpLogFile!=NULL) 
						{
							if(pDumpLogFile->CheckNode(MEMORYDUMPINFO_TYPE_DYNRAM) == FALSE)
							{
								fCorrectVersion = FALSE;
							}
						}
					}
					else fResult = FALSE;
				}
				else fResult = FALSE;
			}

			if ((m_CheckAllBitmaps.GetState() & 0x0003) == TRUE) 
			{					
				CMemoryDumpFileInfo* const ptrFileInfo = 
					m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_ALLBMP);

				if (ptrFileInfo != 0) 
				{
					if (ptrFileInfo->isAvailable()) 
					{
						m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_ALLBMP,TRUE);
					
						if (pDumpLogFile!=NULL) 
						{
							if(pDumpLogFile->CheckNode(MEMORYDUMPINFO_TYPE_ALLBMP) == FALSE)
							{
								fCorrectVersion = FALSE;
							}
						}
					}
					else fResult = FALSE;
				}
				else fResult = FALSE;
			}

			if ((m_CheckAudioBitmaps.GetState() & 0x0003) == TRUE) 
			{				
				CMemoryDumpFileInfo* const ptrFileInfo = 
					m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_AUDIOBMP);

				if (ptrFileInfo != 0) 
				{
					if (ptrFileInfo->isAvailable()) 
					{
						m_ptrMemoryDumpInfo->SetTaskMemoryType(MEMORYDUMPINFO_TYPE_AUDIOBMP,TRUE);
						if (pDumpLogFile!=NULL) 
						{
							if (pDumpLogFile->CheckNode(MEMORYDUMPINFO_TYPE_AUDIOBMP) == FALSE)
							{
								fCorrectVersion = FALSE;
							}
						}
					}
					else fResult = FALSE;
				}
				else fResult = FALSE;
			}
		}


		if (fResult == TRUE) 
		{					
			// HERE WE MUST CHECK NODE SOFTWAREVERSION + LCD TYPES !!!!!
			if (fCorrectVersion == TRUE) 
			{				
				m_ptrMemoryDumpInfo->SetTask(MEMORYDUMPINFO_TASK_DOWNLOAD,FALSE);
				m_ptrMemoryDumpInfo->SetTask(MEMORYDUMPINFO_TASK_UPLOAD,TRUE);
				m_ptrMemoryDumpInfo->SetTask(MEMORYDUMPINFO_TASK_VERIFY,FALSE);
				
				TMGetGlobalHSM()->EvOperationUploadMemory();
				
			}
			else 
			{
				szInfo.Format("Can't upload the requested backup's because they are not compatible with the current software version or node type.");
				AfxMessageBox(szInfo);						 
			}
		}
		else 
		{
			szInfo.Format("Can't perform upload because some files are not available ...");
			AfxMessageBox(szInfo);
		}
	}
}

BOOL CPropertyPageNodeBackup::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();					// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	
	UpdateStaticControls();	
	fEnableMemoryDump = FALSE;
	UpdateControls();

	fInitControls = TRUE;

	RequestRuntimeStatussen();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertyPageNodeBackup::UpdateControls()
{
	CString szStaticRam,szDynRam,szAudioBmpRam,szAllBmp;
	CString szInfo,szTemp,szTemp2;
	CString szMaxErrors;

	int iSelection;

	szMaxErrors.Empty();

	CMemoryDumpFileInfo *ptrFileInfo = NULL;
	CMemoryDumpLog *pLogFile = NULL;


	if (m_ptrMemoryDumpInfo != NULL) 
	{
		if (m_ptrMemoryDumpInfo->GetAssignedNodeAddress() == m_pSelNode->bLogicalAddress) 
		{				
			ptrFileInfo = m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_STATRAM);
			if (ptrFileInfo != NULL) 
			{
				if (ptrFileInfo->isAvailable())
				{
					szStaticRam = ptrFileInfo->GetFileCreationData();
				}
				else 
				{
					szStaticRam.Format(_T("No Backup Available"));	
				}
			}
			else szStaticRam.Format("Error");

		
			ptrFileInfo = m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_DYNRAM);
			if (ptrFileInfo != NULL) 
			{
				if (ptrFileInfo->isAvailable())
				{
					szDynRam = ptrFileInfo->GetFileCreationData();
				}
				else 
				{
					szDynRam.Format(_T("No Backup Available"));	
				}
			}
			else szDynRam.Format("Error");

		
			ptrFileInfo = m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_AUDIOBMP);
			if (ptrFileInfo != NULL) 
			{
				if (ptrFileInfo->isAvailable())
				{
					szAudioBmpRam = ptrFileInfo->GetFileCreationData();
				}
				else 
				{
					szAudioBmpRam.Format(_T("No Backup Available"));	
				}
			}
			else szAudioBmpRam.Format("Error");

			
			ptrFileInfo = m_ptrMemoryDumpInfo->GetFileInfo(MEMORYDUMPINFO_TYPE_ALLBMP);
			if (ptrFileInfo != NULL) 
			{
				if (ptrFileInfo->isAvailable())
				{
					szAllBmp = ptrFileInfo->GetFileCreationData();
				}
				else 
				{
					szAllBmp.Format(_T("No Backup Available"));	
				}
			}
			else szAllBmp.Format("Error");


			// .......................................................

			iSelection = m_CmbMemoryType.GetCurSel();

			// .......................................................

			
			// .......... Update Edit Box .............................
			
			
			CMemoryDumpInfoVerify *pVerifyInfo = NULL;
			pVerifyInfo = m_ptrMemoryDumpInfo->GetConfigVerifyInfo((ENUM_MEMORYDUMPINFO_TYPE) iSelection);
			if (pVerifyInfo != NULL) 
			{
				szMaxErrors.Format(_T("%d"),pVerifyInfo->Get_VerifyErrors());
			}


			// .......... Update Static text Info ......................		
			pLogFile = m_ptrMemoryDumpInfo->GetLogFile((ENUM_MEMORYDUMPINFO_TYPE) iSelection);
			szInfo = szTemp;
			if (pLogFile!=NULL) 
			{
				szTemp2.Empty();

				if (pLogFile->IsInfoAvailable()) 
				{				
					ptrFileInfo = m_ptrMemoryDumpInfo->GetFileInfo((ENUM_MEMORYDUMPINFO_TYPE) iSelection);
					if (ptrFileInfo != NULL) 
					{
						if (ptrFileInfo->isAvailable())
						{		 
							szTemp.Format(_T("File creation date:\n%s\n"),ptrFileInfo->GetFileCreationData());
							szTemp2 += szTemp;
						}
					}				
						
					szTemp.Format(_T("\nSoftware Version: v%02x\n"),
								  pLogFile->Get_SoftwareVersion());
					szTemp2 += szTemp;

					szTemp.Format(_T("Physical Address: 0x%06x\n"),
								   pLogFile->Get_PhysicalAddress());

					szTemp2 += szTemp;					

					switch(pLogFile->Get_LcdType()) 
					{
						case NODE_LCD_TYPE_BASIC: 
							szTemp.Format(_T("LCD type: Basic LCD\n")); 
						break;
						case NODE_LCD_TYPE_HC4 : 
							szTemp.Format(_T("LCD type: HC4 LCD\n")); 
						break;
						case NODE_LCD_TYPE_HC5 : 
							szTemp.Format(_T("LCD type: HC5 LCD\n")); 
						break;
						default : 
							szTemp.Format(_T("LCD type: Unknown")); 
						break;
					}
					szTemp2 += szTemp;
				
					szTemp.Format(_T("\nNr Verify errors: %d\n"),pLogFile->Get_nVerifyErrors());
					szTemp2 += szTemp;

					szTemp.Format(_T("Nr memory errors: %d\n"),pLogFile->Get_nMemoryErrors());
					szTemp2 += szTemp;

					szInfo = szTemp2;
				}
				else 
				{
					szInfo.Format(_T("No backup Available"));
				}			
			}
			else
			{
				szInfo.Format(_T("No Info Available..."));
			}		
		}
		else 
		{
			szStaticRam.Format("Error - other node");
			szDynRam.Format("Error - other node");
			szAudioBmpRam.Format("Error - other node");
			szAllBmp.Format("Error - other node");

			szInfo.Format(_T("No Information available\nMemory is assigned to another node"));
		}
	}
	else
	{
		szStaticRam.Format("Error");
		szDynRam.Format("Error");
		szAudioBmpRam.Format("Error");
		szAllBmp.Format("Error");

		szInfo.Format(_T("Error - Pointer to NULL"));
	}

	m_EditStatic.SetWindowText(szStaticRam);
	m_EditDynamic.SetWindowText(szDynRam);	
	m_EditAudioBmp.SetWindowText(szAudioBmpRam);
	m_EditBitmaps.SetWindowText(szAllBmp);

	m_StaticInfo.SetWindowText(szInfo);

	if (szMaxErrors.IsEmpty() == FALSE) 
	{
		m_EditMaxErrors.SetWindowText(szMaxErrors);
	}

	m_BtnUpload.EnableWindow(fEnableMemoryDump);
	m_BtnDownload.EnableWindow(fEnableMemoryDump);
}

void CPropertyPageNodeBackup::UpdateStaticControls()
{
	if ((m_pSelNode->GetSoftwareVersion() >= 0x64) && 
		(!((m_pSelNode->bNodeFlags & NODE_LCD_TYPE_MASK)==NODE_LCD_TYPE_BASIC)))
	{
		m_CheckAudioBitmaps.EnableWindow(TRUE);
	}
	else 
	{
		m_CheckAudioBitmaps.EnableWindow(FALSE);
	}

	m_CmbMemoryType.InsertString((int)MEMORYDUMPINFO_TYPE_DYNRAM,"Dynamic Memory");
	m_CmbMemoryType.InsertString((int)MEMORYDUMPINFO_TYPE_STATRAM,"Static Memory");
	m_CmbMemoryType.InsertString((int)MEMORYDUMPINFO_TYPE_ALLBMP,"Bitmap Memory");
	m_CmbMemoryType.InsertString((int)MEMORYDUMPINFO_TYPE_AUDIOBMP,"Audio Bitmap Memory");

	m_CmbMemoryType.SetCurSel(0);
}

void CPropertyPageNodeBackup::OnCloseupComboMemory() 
{
	UpdateControls();
}

void CPropertyPageNodeBackup::OnButtonSetmaxerrors() 
{
	CString szInfo;
	int val;

	if (m_ptrMemoryDumpInfo != NULL) 
	{
		if (m_ptrMemoryDumpInfo->IsLocked() == FALSE) 
		{		
			const int iSel = m_CmbMemoryType.GetCurSel();

			CMemoryDumpInfoVerify* const pVerifyInfo = 
				m_ptrMemoryDumpInfo->GetConfigVerifyInfo((ENUM_MEMORYDUMPINFO_TYPE)iSel);

			if (pVerifyInfo != NULL) 
			{		
				CString szEdit;

				m_EditMaxErrors.GetWindowText(szEdit);
				val = 0;

				sscanf(szEdit,"%d",&val);		// CHECKED_SSCANF_OK

				if (val >=0 && val<100) 
				{
					szInfo.Format("Are you sure you want to change this value\n"
								  "and possible store a memory backup that is corrupt?");

					if (AfxMessageBox(szInfo,MB_ICONQUESTION | MB_YESNO) == IDYES) {
				
						// Set Value ...
						pVerifyInfo->Set_VerifyErrors(val);

						UpdateControls();
					
					}	
				}
				else 
				{
					szInfo.Format("Value is too large");
					AfxMessageBox(szInfo);
				}
			}
		}	
	}
}

// UpdateRunTimeControls may only be invoked when node responds to 
// commands that identify LCD Type + Software Version ...
void CPropertyPageNodeBackup::UpdateRunTimeControls(void)
{	
	if (fEnableMemoryDump == FALSE) 
	{		
		fEnableMemoryDump = TRUE;
		
		if (fInitControls) UpdateControls();	
	}
}

void CPropertyPageNodeBackup::RequestRuntimeStatussen()
{
	if (m_ptrMemoryDumpInfo != 0) 
	{
		if (m_ptrMemoryDumpInfo->IsLocked() == FALSE) 
		{		
			ASSERT( m_pSelNode );
			
			CTXBrowsePnpMessages::getInstance()->
				RequestNodeConfig(m_pSelNode->bLogicalAddress);

			CTXBrowsePnpMessages::getInstance()->
				RequestNodeResources(m_pSelNode->bLogicalAddress,2);		
		}
	}
}




