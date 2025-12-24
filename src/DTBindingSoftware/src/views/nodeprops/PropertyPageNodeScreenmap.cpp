// PropertyPageNodeScreenmap.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"
#include "PropertyPageNodeScreenmap.h"
#include "NodeDatabase.h"
#include "MainFrm.h"
#include "MyWindowMessages.h"
#include "NodeLCDScreenmapFSMInfo.h"
#include "nodelcdscreenmaps_util.h"
#include "screenmaps.h"
#include "screenmapid.h"
#include "NodeDatabaseOperations.h"
#include "FactoryNodeLCDScreenmaps.h"
#include "App\AppCfg\AppCfgsmap\configsmapsmapinfo.h"
#include "app\appnodedbase\appnodedbase.h"

#include "utils/app/DisplayDevice.h"

// ------------------------------------
#include "TMGlobals.h"
using namespace TMCommunication;
using namespace TMGlobalHSM;
// ------------------------------------


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
CEditSubClass::CEditSubClass()
{
   	iLineNR = 0;
	iMaxLineNr = 0;
	iPageNR = 0;
	iMaxPageNr = 0;
	iButtonNR = 0;	
	iButtonsPerPage = 9;
}

BEGIN_MESSAGE_MAP(CEditSubClass, CEdit)
    ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


void CEditSubClass::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UpdateParentControls();
	CEdit::OnLButtonUp(nFlags, point);
}



// We Must do our own cut & paste routnes ...

void CEditSubClass::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	UpdateParentControls();
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CEditSubClass::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	UpdateParentControls();
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CEditSubClass::UpdateParentControls(void)
{	
	iLineNR = LineFromChar();
	iMaxLineNr = GetLineCount();

	iButtonNR = iLineNR % iButtonsPerPage;
	iPageNR = iLineNR / iButtonsPerPage;
	iMaxPageNr = ((iMaxLineNr-1)/iButtonsPerPage);

	CWnd *pWnd = NULL;
	if ((pWnd = GetOwner()) != NULL) 
	{
		((CPropertyPageNodeScreenmap*)pWnd)->UpdateLineNr(iLineNR,iMaxLineNr);
		((CPropertyPageNodeScreenmap*)pWnd)->UpdatePageNr(iPageNR,iMaxPageNr);
		((CPropertyPageNodeScreenmap*)pWnd)->UpdateButtonNr(iButtonNR,iButtonsPerPage);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeScreenmap property page

IMPLEMENT_DYNCREATE(CPropertyPageNodeScreenmap, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage

CPropertyPageNodeScreenmap::CPropertyPageNodeScreenmap() : 
CPropertyPageNode(CPropertyPageNodeScreenmap::IDD),
m_pNodeDatabase(0),
m_pNodeLcdScreenmaps( 0 )
{
	//{{AFX_DATA_INIT(CPropertyPageNodeScreenmap)
	m_RadioDimmer = -1;
	m_StringContent = _T("");
	//}}AFX_DATA_INIT

	ASSERT( m_pNodeDatabase );
	ASSERT( m_pNodeLcdScreenmaps );
}

CPropertyPageNodeScreenmap::~CPropertyPageNodeScreenmap()
{
	TRACE("CPropertyPageNodeScreenmap::~CPropertyPageNodeScreenmap()\n");

	if (m_pNodeLcdScreenmaps)
	{
		delete m_pNodeLcdScreenmaps;
	}
}

CPropertyPageNodeScreenmap::CPropertyPageNodeScreenmap(CCanNode *pNode,CNodeDatabase *pDatabase,CString s) 
: CPropertyPageNode(CPropertyPageNodeScreenmap::IDD,pNode),
m_pNodeDatabase(pDatabase),
m_pNodeLcdScreenmaps(CFactoryNodeLCDScreenmaps::create(pDatabase, m_pSelNode)),
m_szPathName(s)
{
	ASSERT(m_pSelNode);
	ASSERT(m_pNodeDatabase);
	ASSERT(m_pNodeLcdScreenmaps );
	ASSERT(m_szPathName.IsEmpty() == FALSE);
}


void CPropertyPageNodeScreenmap::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyPageNodeScreenmap)
	DDX_Control(pDX, IDC_STATIC_START, m_StaticStart);
	DDX_Control(pDX, IDC_EDIT_START, m_editStart);
	DDX_Control(pDX, IDC_COMBO_COPYTO, m_ComboCopyTo);
	DDX_Control(pDX, IDC_BUTTON_COPY, m_BtnCopy);
	DDX_Control(pDX, IDC_STATIC_PAGE, m_StaticPageNr);
	DDX_Control(pDX, IDC_STATIC_LINE, m_StaticLineNr);
	DDX_Control(pDX, IDC_STATIC_BUTTON, m_StaticButtonNr);
	DDX_Radio(pDX, IDC_RADIO_DIMMERS, m_RadioDimmer);
	DDX_Text(pDX, IDC_EDIT_SCRMAPBOX, m_StringContent);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropertyPageNodeScreenmap, CPropertyPage)		// CHANGES_MVS_2008 : Afleiden van CPropertyPage
	//{{AFX_MSG_MAP(CPropertyPageNodeScreenmap)
	ON_BN_CLICKED(IDC_BUTTON_INIT, OnButtonInit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_ALL, OnButtonUpdateAll)
	ON_EN_KILLFOCUS(IDC_EDIT_START, OnKillfocusEditStart)
	ON_EN_KILLFOCUS(IDC_EDIT_SCRMAPBOX, OnKillfocusEditScrmapbox)
	//}}AFX_MSG_MAP

	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_DIMMERS,IDC_RADIO_AUDIO,OnRadioButtonClicked)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageNodeScreenmap message handlers

void CPropertyPageNodeScreenmap::OnRadioButtonClicked(UINT nCmdID)
{
	// since 0x0571:
	// Screenmaps worden gecontroleerd en opgeslaan in CScreenmaps klasse in de 
	// functie: OnKillfocusEditScrmapbox

	if (m_pNodeLcdScreenmaps)
	{
		m_ptrScreenmap = NULL;

		switch (nCmdID)
		{
			case IDC_RADIO_DIMMERS:
			{
				screenmapChoice = enum_screenmap_dimmers;				

				m_ptrScreenmap = m_pNodeLcdScreenmaps->FindScreenmap(CScreenmapID(SCREENMAP_DIMMER));
				ASSERT(m_ptrScreenmap);

			} break;	

			case IDC_RADIO_SWITCH :
			{
				screenmapChoice = enum_screenmap_switch;			

				m_ptrScreenmap = m_pNodeLcdScreenmaps->FindScreenmap(CScreenmapID(SCREENMAP_SWITCH));
				ASSERT(m_ptrScreenmap);

			} break;

			case IDC_RADIO_MOODS :
			{
				screenmapChoice = enum_screenmap_moods;			

				m_ptrScreenmap = m_pNodeLcdScreenmaps->FindScreenmap(CScreenmapID(SCREENMAP_CONTROLS));
				ASSERT(m_ptrScreenmap);
			
			} break;

			case IDC_RADIO_SENS :
			{
				screenmapChoice = enum_screenmap_sens;
				
				m_ptrScreenmap = m_pNodeLcdScreenmaps->FindScreenmap(CScreenmapID(SCREENMAP_SENS));
				ASSERT(m_ptrScreenmap);

			} break;

			case IDC_RADIO_DUOSWITCH :
			{
				screenmapChoice = enum_screenmap_duoswitch;

				m_ptrScreenmap = m_pNodeLcdScreenmaps->FindScreenmap(CScreenmapID(SCREENMAP_DUOSWITCH));			
				ASSERT(m_ptrScreenmap);

			} break;

			case IDC_RADIO_AUDIO :
			{
				screenmapChoice = enum_screenmap_audio;

				m_ptrScreenmap = m_pNodeLcdScreenmaps->FindScreenmap(CScreenmapID(SCREENMAP_AUDIO));			
				ASSERT(m_ptrScreenmap);

			} break;

			default:
			{
				AfxMessageBox("Error : CPropertyPageNodeScreenmap::OnRadioButtonClicked : Unknown command received");

				m_ptrScreenmap = NULL; 

			} break;
		}

		if (m_ptrScreenmap != NULL)
		{
			UpdateScreenmapChoice( );
			UpdateScreenmapDefaultPageNR( );
		}
		else
		{
			CString szError(_T("Fatal Error in CPropertyPageNodeScreenmap::OnRadioButtonClicked occured\n"
							   "Pointer to Screenmap is NULL"));
			
			AfxMessageBox(szError);
		}
	}
	else 
	{		// Store Screenmap Failed !!!!
		m_EditBox.GetWindowText(m_StringContent);
		UpdateData(FALSE);		
	} 
}

BOOL CPropertyPageNodeScreenmap::OnInitDialog() 
{	
	CPropertyPage::OnInitDialog();

	m_ptrScreenmap = 0;	
	screenmapChoice = enum_screenmap_dimmers;
	m_RadioDimmer = screenmapChoice;
	
	UpdateData(FALSE);

	m_EditBox.SubclassDlgItem(IDC_EDIT_SCRMAPBOX, this);  

	if (m_pNodeDatabase && m_pNodeLcdScreenmaps) 
	{		
		loadScreenmaps( );
		
		if ((m_ptrScreenmap = m_pNodeLcdScreenmaps->FindScreenmap(CScreenmapID(SCREENMAP_DIMMER))) == NULL) 
		{
			CString szError(_T("Fatal Error in CPropertyPageNodeScreenmap::OnInitDialog occured\n"
						       "Pointer to Screenmap is NULL"));
			AfxMessageBox(szError);
		}


		UpdateScreenmapChoice();
		UpdateScreenmapDefaultPageNR( );
		UpdateCopyScreenmapControls();						
	}
	else 
	{
		CString szError(_T("Fatal Error in CPropertyPageNodeScreenmap::OnInitDialog occured\n"
						   "Pointer to NodeDatabase is NULL"));
		AfxMessageBox(szError);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//------------------------------------------------------------
void CPropertyPageNodeScreenmap::UpdateLineNr(int i,int max)
{
	CString szText;
	szText.Format("Line nr. : %d/%d",i+1,max);
	m_StaticLineNr.SetWindowText(szText);
}

void CPropertyPageNodeScreenmap::UpdatePageNr(int i,int max)
{
	CString szText;	
	szText.Format("Page nr. : %d/%d",i+1,max+1);
	m_StaticPageNr.SetWindowText(szText);
}

void CPropertyPageNodeScreenmap::UpdateButtonNr(int i,int max)
{
	CString szText;
	szText.Format("Button nr. : %d/%d",i+1,max);
	m_StaticButtonNr.SetWindowText(szText);
}


BOOL CPropertyPageNodeScreenmap::OnKillActive() 
{
	// saveifModified( );

	return CPropertyPage::OnKillActive();
}

void CPropertyPageNodeScreenmap::OnButtonInit() 
{
	if (m_ptrScreenmap != NULL) 
	{
		m_ptrScreenmap->FillStandard(m_pSelNode);	
		m_ptrScreenmap->setStartPage(0);			
		UpdateScreenmapChoice();	
		UpdateScreenmapDefaultPageNR( );
		m_ptrScreenmap->SetModified(TRUE);
	}
}

BOOL CPropertyPageNodeScreenmap::UpdateScreenmapChoice(void)
{
	if (m_ptrScreenmap == NULL) 
	{
		return FALSE;
	}

	m_StringContent.Empty();

	m_ptrScreenmap->UpdateNames();

	m_ptrScreenmap->ConvertToString(&m_StringContent);
	m_RadioDimmer = screenmapChoice;
	
	UpdateData(FALSE);

	// must be placed here after that UpdateData(False) has been invoked !!!
	m_EditBox.SetButtonsPerPage(m_ptrScreenmap->GetButtonsPerPage());
	m_EditBox.UpdateParentControls();

	return TRUE;
}

BOOL CPropertyPageNodeScreenmap::UpdateScreenmapDefaultPageNR( )
{	
	if (m_ptrScreenmap == NULL) 
	{
		return FALSE;
	}

	if (m_pSelNode &&
		NODESCREENMAP_CONFIG_TYPE_V65 == CNodeScreenmapInfo_Util::getNodeType(m_pSelNode))		
	{
		m_editStart.ShowWindow( TRUE );
		m_StaticStart.ShowWindow( TRUE );
		
		int value = m_ptrScreenmap->getStartPage() + 1;
		if (value <= 0)
		{
			value = 1;
		}

		CString szText;
		szText.Format("%d",value);
		m_editStart.SetWindowText(szText);

	}
	else
	{
		m_editStart.ShowWindow( FALSE );
		m_StaticStart.ShowWindow( FALSE );
	}
	return TRUE;
}

BOOL CPropertyPageNodeScreenmap::StoreScreenmapChoice(void)
{
	if (m_ptrScreenmap == NULL)
	{
		return FALSE;
	}

	if (m_EditBox.GetSafeHwnd() != 0)
	{
		m_EditBox.GetWindowText(m_StringContent);

		const int iLength = m_StringContent.GetLength();

		CScreenmapUtil::NormalizeString(&m_StringContent);		// since v0564

		CString szError;
		const BOOL fResult = CScreenmapUtil::CheckStringForErrors(m_ptrScreenmap,
																  m_StringContent.GetBuffer(iLength),
						  										  iLength,
																  &szError);

		if (fResult == TRUE) 
		{	
			szError.Empty();
			CScreenmapUtil::ConvertStringToStructs(m_ptrScreenmap,
												   m_StringContent.GetBuffer(iLength),
						  						   iLength,
												   &szError);

			if (szError.IsEmpty()) 
			{
				m_ptrScreenmap->UpdateNames();			
				return TRUE;
			}
			else 
			{
				AfxMessageBox(szError);	
				return FALSE;
			}	

			return TRUE;
		}
		else 
		{
			AfxMessageBox(szError);	
			return FALSE;
		}
	}	
	return FALSE;
}

void CPropertyPageNodeScreenmap::OnButtonSave() 
{	
	if (m_EditBox.GetModify() == TRUE && 
		StoreScreenmapChoice()) 
	{									
		m_EditBox.SetModify(FALSE);						
	}

	saveScreenmaps( );
}

void CPropertyPageNodeScreenmap::OnButtonLoad() 
{
	loadScreenmaps( );

	UpdateScreenmapChoice();
	UpdateScreenmapDefaultPageNR( );
}


#if(USE_SMAP_ENERGYMEASURE == 1)

// Aanpassingen V16.21: Screenmapping van de virtuele energie meet units.
// Gebruik van een CNodeLCDScreenmapFSMInfo_Util klasse.
void CPropertyPageNodeScreenmap::OnButtonUpdate() 
{	
	if (StoreScreenmapChoice() && m_pNodeLcdScreenmaps) 
	{	
		saveScreenmaps( );

		CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
		if (pRefScreenmapsUpdateInfo)
		{
			if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE)
			{
				if (AfxGetMainWnd() != NULL)
				{
					CString strError;
					pRefScreenmapsUpdateInfo->Clear();
				
					if (!CNodeLCDScreenmapFSMInfo_Util::addNode(pRefScreenmapsUpdateInfo, m_pSelNode, CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS, &strError)) {
						AfxMessageBox(strError);
						return;
					}													
					
					TMGetGlobalHSM()->EvOperationUploadScreenmaps();
				}
			}
			else
			{
				CString szError("Screenmaps FSM is locked");
				AfxMessageBox(szError);
			}
		}
	}
}

// Aanpassingen V16.21: Screenmapping van de virtuele energie meet units.
// Gebruik van een CNodeLCDScreenmapFSMInfo_Util klasse.
void CPropertyPageNodeScreenmap::OnButtonUpdateAll() 
{
	if (StoreScreenmapChoice() && m_pNodeLcdScreenmaps) 
	{		
		saveScreenmaps( );

		CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		

		if (pRefScreenmapsUpdateInfo)
		{
			if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE)
			{							
				CNodeDatabaseOperations NodeDatabaseOperations(m_pNodeDatabase);

				CPtrList List;
				const int nCount = NodeDatabaseOperations.SubclassNodes(NODE_LCD,&List);
				if (nCount != 0)
				{
					if (AfxGetMainWnd() != NULL)
					{
						pRefScreenmapsUpdateInfo->Clear();

						POSITION pos = List.GetHeadPosition();
						while(pos!=NULL)
						{
							CCanNode *pNode = 0;
							if ((pNode = (CCanNode*) List.GetNext(pos)) != NULL)
							{
								if (CNodeScreenmapInfo_Util::isSupported(pNode))
								{
									CString strError;
									if (!CNodeLCDScreenmapFSMInfo_Util::addNode(pRefScreenmapsUpdateInfo, pNode, CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS, &strError)) {
										AfxMessageBox(strError);
										return;
									}											
								}
							}
						}												
						TMGetGlobalHSM()->EvOperationUploadScreenmaps();
					}
				}			
			}
			else
			{
				CString szError("Screenmaps FSM is locked");
				AfxMessageBox(szError);
			}
		}
	}
}

#else

void CPropertyPageNodeScreenmap::OnButtonUpdate() 
{	
	if (StoreScreenmapChoice() && m_pNodeLcdScreenmaps) 
	{	
		saveScreenmaps( );

		CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		

		if (pRefScreenmapsUpdateInfo)
		{
			if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE)
			{
				if (AfxGetMainWnd() != NULL)
				{
					pRefScreenmapsUpdateInfo->Clear();
					pRefScreenmapsUpdateInfo->AddNodeToSmapFsm(m_pSelNode);			
					
					// Sedert v0571: Om te bepalen welke screenmaps er moeten doorgestuurd worden.

					pRefScreenmapsUpdateInfo->setType( CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS );
					pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_DIMMER));
					pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_SWITCH));
					pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_CONTROLS));
					pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_SENS));
					pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_DUOSWITCH));
					pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_AUDIO));
					
					TMGetGlobalHSM()->EvOperationUploadScreenmaps();
				}
			}
			else
			{
				CString szError("Screenmaps FSM is locked");
				AfxMessageBox(szError);
			}
		}
	}
}

void CPropertyPageNodeScreenmap::OnButtonUpdateAll() 
{
	if (StoreScreenmapChoice() && m_pNodeLcdScreenmaps) 
	{		
		saveScreenmaps( );

		CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		

		if (pRefScreenmapsUpdateInfo)
		{
			if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE)
			{							
				CNodeDatabaseOperations NodeDatabaseOperations(m_pNodeDatabase);

				CPtrList List;
				const int nCount = NodeDatabaseOperations.SubclassNodes(NODE_LCD,&List);
				if (nCount != 0)
				{
					if (AfxGetMainWnd() != NULL)
					{
						pRefScreenmapsUpdateInfo->Clear();

						POSITION pos = List.GetHeadPosition();
						while(pos!=NULL)
						{
							CCanNode *pNode = 0;
							if ((pNode = (CCanNode*) List.GetNext(pos)) != NULL)
							{
								if (CNodeScreenmapInfo_Util::isSupported(pNode))
								{
									pRefScreenmapsUpdateInfo->AddNodeToSmapFsm(pNode);							
								}
							}
						}	
						
						// Sedert v0571: Om te bepalen welke screenmaps er moeten doorgestuurd worden.				

						// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						// OPPASSEN: mogen hier geen speciale V65 SCREENMAPS doorsturen want de 
						// V64 Nodes ondersteunen dit niet en kunnen corrupt worden 
						// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

						pRefScreenmapsUpdateInfo->setType( CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS );
						pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_DIMMER));
						pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_SWITCH));
						pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_CONTROLS));
						pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_SENS));
						pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_DUOSWITCH));
						pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_AUDIO));
						TMGetGlobalHSM()->EvOperationUploadScreenmaps();
					}
				}			
			}
			else
			{
				CString szError("Screenmaps FSM is locked");
				AfxMessageBox(szError);
			}
		}
	}
}
#endif 


/**
 *
 */
void CPropertyPageNodeScreenmap::OnButtonCopy() 
{
	ASSERT(m_pSelNode);
	ASSERT(m_pNodeDatabase);

	if (m_pSelNode != NULL && m_pNodeDatabase != NULL)
	{
		int nCurSel;

		if ((nCurSel= m_ComboCopyTo.GetCurSel()) != CB_ERR)
		{
			if (m_ComboCopyTo.GetItemDataPtr(nCurSel) == m_pNodeDatabase)
			{
				CString szMessage;
				szMessage.Format("Are you sure you want to copy the screenmaps from '%s' to "
								 "all LCD Nodes",
								 m_pSelNode->szNodeName);
				
				if (AfxMessageBox(szMessage,MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					if (CopyScreenmaps(NULL) == FALSE)
					{
						ASSERT(0);
					}
				}
			}
			else
			{
				CCanNode *pNode = static_cast<CCanNode*> 
					(m_ComboCopyTo.GetItemDataPtr(nCurSel));

				CString szMessage;
				szMessage.Format("Are you sure you want to copy the screenmaps from '%s' to "
								 "LCD Node '%s'",								 
								 m_pSelNode->szNodeName,
								 pNode->szNodeName);

				if (AfxMessageBox(szMessage,MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					//CopyTo(pNode);
					if (CopyScreenmaps(pNode) == FALSE)
					{
						ASSERT(0);
					}
				}
			}		
		}
	}
}

BOOL CPropertyPageNodeScreenmap::CopyScreenmaps(CCanNode *pNode)
{
	BOOL fResult = FALSE;

	// Set Cursor Wait ...
	HCURSOR hOldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));

	CDisplayDevice::Clear();

	if (1)
	{
		CString szMessage("Busy copying Screenmaps ...");
		CDisplayDevice::DisplayInfoMsg(szMessage);
	}

	if (pNode == NULL)
	{
		/**
		 * Attention First index in combobox is poiner to CNodeDatabase !!!!
		 */
		const int nCount = m_ComboCopyTo.GetCount();		
		
		for (int i=1;i<nCount;i++)
		{
			CCanNode *_pNode = static_cast<CCanNode*> (m_ComboCopyTo.GetItemDataPtr(i));	
			
			if (_pNode != NULL)
			{
				CNodeLcdScreenmaps* const pDestScreenmap = CFactoryNodeLCDScreenmaps::create(m_pNodeDatabase, _pNode);

				if (pDestScreenmap)
				{
					// pDestScreenmap->Init(m_pNodeDatabase);
					
					if ((fResult = CNodeLcdScreenmaps::Copy(pDestScreenmap,
										  				    m_pNodeLcdScreenmaps)) 
															== TRUE)				
					{					
						/**
						 * Save Destination Screenmaps !
						 */
						pDestScreenmap->Save(m_szPathName);

						CString szMessage;
						szMessage.Format("Copy screenmaps from node '%s' to '%s' and save",
										 m_pSelNode->szNodeName,
										 _pNode->szNodeName);
						
						CDisplayDevice::DisplayInfoMsg(szMessage);
					}
					else
					{
						CString szMessage;
						szMessage.Format("Copy screenmaps from node '%s' to '%s' FAILED",
										 m_pSelNode->szNodeName,
										 _pNode->szNodeName);
						
						CDisplayDevice::DisplayErrorMsg(szMessage);
					}
				}

				delete pDestScreenmap;
			}
		}		
	}
	else
	{
		CNodeLcdScreenmaps* const pDestScreenmap = CFactoryNodeLCDScreenmaps::create(m_pNodeDatabase, pNode);
		if (pDestScreenmap)
		{
			//pDestScreenmap->Init(m_pNodeDatabase);
						
			if ((fResult = CNodeLcdScreenmaps::Copy(pDestScreenmap,
													m_pNodeLcdScreenmaps)) 
													== TRUE)
			{					
				/**
				 * Save Destination Screenmaps !
				 */
				pDestScreenmap->Save(m_szPathName);

				CString szMessage;
				szMessage.Format("Copy screenmaps from node '%s' to '%s' and save",
								 m_pSelNode->szNodeName,
								 pNode->szNodeName);
							
				CDisplayDevice::DisplayInfoMsg(szMessage);	
			}
			else
			{
				CString szMessage;
				szMessage.Format("Copy screenmaps from node '%s' to '%s' FAILED",
								 m_pSelNode->szNodeName,
								 pNode->szNodeName);
							
				CDisplayDevice::DisplayErrorMsg(szMessage);	
			}

			delete pDestScreenmap;
		}
	}

	CString szMessage("Copy Screenmaps done ...");
	CDisplayDevice::DisplayInfoMsg(szMessage);

	// Restore Cursor ...
	if (hOldCursur != NULL)
	{
		SetCursor(hOldCursur);
	}

	return fResult;
}



/**
 * Fill combo box with Nodes of Type NODE_LCD.
 * First item is ALWAYS ptr to CNodeDatabase --> ALL LCD's.
 */
void CPropertyPageNodeScreenmap::UpdateCopyScreenmapControls()
{	
	bool enabled = false;

	m_BtnCopy.EnableWindow( FALSE );
	m_ComboCopyTo.EnableWindow( FALSE );	

	CNodeDatabaseOperations NodeDatabaseOperations(m_pNodeDatabase);
	CPtrList List;
	const int nCount = NodeDatabaseOperations.SubclassNodes(NODE_LCD,&List);
	if (nCount > 1)
	{				
		const NodeScreenmapConfigtype screenmapConfigType =
			CNodeScreenmapInfo_Util::getNodeType( m_pSelNode );

		POSITION pos = List.GetHeadPosition();
		while(pos!=NULL)
		{
			CCanNode *pNode;
			if ((pNode = (CCanNode*) List.GetNext(pos)) != NULL)
			{
				if (pNode != m_pSelNode &&
					CNodeScreenmapInfo_Util::getNodeType(pNode) == screenmapConfigType)
				{			
					enabled = true;
				}
			}
		}

		if (enabled)
		{
			m_BtnCopy.EnableWindow(TRUE);
			m_ComboCopyTo.EnableWindow(TRUE);	
				
			/** Index 0 = Ptr to CNodeDatabase, All LCD's. */
			m_ComboCopyTo.InsertString(0, "All LCD's");
			m_ComboCopyTo.SetItemDataPtr(0, m_pNodeDatabase);
			
			int index = 1;		// Index 0 is copy to all LCDs
			pos = List.GetHeadPosition();
			while(pos!=NULL)
			{
				CCanNode *pNode;
				if ((pNode = (CCanNode*) List.GetNext(pos)) != NULL)
				{
					if (pNode != m_pSelNode &&
						CNodeScreenmapInfo_Util::getNodeType(pNode) == screenmapConfigType)
					{				
						if ((index = m_ComboCopyTo.InsertString(index,pNode->szNodeName)) != CB_ERR)
						{
							m_ComboCopyTo.SetItemDataPtr(index,pNode);
						}
						else
						{
							ASSERT(0);
							break;
						}
					}
				}				
			}			
			/** Copy to all LCD's --> Default ...*/
			m_ComboCopyTo.SetCurSel(0);
		}
	}
}


void CPropertyPageNodeScreenmap::OnKillfocusEditStart() 
{
	if (m_pSelNode)
	{
		if (NODESCREENMAP_CONFIG_TYPE_V65 == 
			CNodeScreenmapInfo_Util::getNodeType(m_pSelNode))
		{
			CString szBuffer;
			int tempValue = 0;

			m_editStart.GetWindowText(szBuffer);
					
			if (sscanf(szBuffer,"%d",&tempValue))		// CHECKED_SSCANF_OK
			{
				if (tempValue > 0)
				{
					int value = tempValue - 1;			
					if (value != m_ptrScreenmap->getStartPage())
					{
						if (value <= 255)
						{
							CConfigSmapSmapInfo& refConfigSmapSmapInfo =
								CConfigSmapSmapInfo::getInstance();

							m_ptrScreenmap->setStartPage( value );
							m_ptrScreenmap->SetModified( TRUE );

							CNodeLcdScreenmaps_Util::update_StartPage(
								*m_pNodeLcdScreenmaps,
								&refConfigSmapSmapInfo);													
						}
					}
				}
				else
				{
					CString szErrorMessage;
					szErrorMessage.Format("Error: Wrong value '%d' for start page property",tempValue);
					AfxMessageBox(szErrorMessage);
				}
			}
			else
			{
				CString szErrorMessage("Error: Wrong value for start page property");	
				AfxMessageBox(szErrorMessage);
			}
		}
	}	
}

// private

BOOL CPropertyPageNodeScreenmap::saveScreenmaps( )
{
	BOOL result = FALSE;

	if (m_pNodeLcdScreenmaps)
	{
		m_pNodeLcdScreenmaps->Save(m_szPathName);	
	}


	if (m_pSelNode)	
	{
		if (NODESCREENMAP_CONFIG_TYPE_V65 == 
			CNodeScreenmapInfo_Util::getNodeType(m_pSelNode))
		{			
			CConfigSmapSmapInfo& refConfigSmapInfo = CConfigSmapSmapInfo::getInstance();
			const BYTE bNodeAddress = m_pSelNode->getNodeAddress();

			CNodeLcdScreenmaps_Util::update_StartPage(															
									*m_pNodeLcdScreenmaps,
									&refConfigSmapInfo);	

			refConfigSmapInfo.save( bNodeAddress );			
		}
	}

	return result;
}

BOOL CPropertyPageNodeScreenmap::loadScreenmaps( )
{
	if (m_pNodeLcdScreenmaps)
	{
		m_pNodeLcdScreenmaps->Load(m_szPathName);
	}

	if (m_pSelNode && m_pNodeLcdScreenmaps)	
	{	
		if (NODESCREENMAP_CONFIG_TYPE_V65 == 
			CNodeScreenmapInfo_Util::getNodeType(m_pSelNode))
		{	
			CConfigSmapSmapInfo& refConfigSmapInfo = CConfigSmapSmapInfo::getInstance();
			const BYTE bNodeAddress = m_pSelNode->getNodeAddress();

			if (refConfigSmapInfo.load(bNodeAddress))
			{		
				// Screenmaps worden geaved ...
				CNodeLcdScreenmaps_Util::update_StartPage(
								refConfigSmapInfo,								
								m_pNodeLcdScreenmaps);	
			}
		}
	}					
	return TRUE;
}

BOOL CPropertyPageNodeScreenmap::saveifModified( )
{
	BOOL fRequestSave = FALSE;

	if (m_pNodeLcdScreenmaps)
	{
		if (m_pNodeLcdScreenmaps->IsModified())
		{	
			fRequestSave = TRUE;
		}	
	}

	if (fRequestSave)
	{
		const CString szMessage(_T("Would you like to save the modified screenmaps ?"));		
		if (AfxMessageBox(szMessage,MB_YESNO) == IDYES) 
		{
			saveScreenmaps( );			
			return TRUE;
		}	
	}
	return FALSE;
}



void CPropertyPageNodeScreenmap::PostNcDestroy() 
{	
	saveifModified( );

	CPropertyPage::PostNcDestroy();
}

void CPropertyPageNodeScreenmap::OnKillfocusEditScrmapbox() 
{
	if (m_EditBox.GetModify() == TRUE) 
	{		
		const BOOL fResult = StoreScreenmapChoice();
		if (fResult == TRUE) 
		{					
			m_ptrScreenmap->SetModified(TRUE);
			m_EditBox.SetModify(FALSE);				
		}
	}
}
