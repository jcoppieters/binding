// PropertyPageNodeSmapLite.cpp : implementation file
//

#include "stdafx.h"
#include "BindingSoftware.h"		// Nodig voor de resources.
#include "PropertyPageNodeSmapLite.h"

#include "nodedatabaseoperations.h"
#include "nodescreenmapinfo.h"

#include "CanNode.h"

#include "app/appcfg/appcfgsmaplite/appcfgsmapliteinfo.h"
#include "app/appcfg/appcfgsmaplite/appcfgsmapliteclass.h"

#include "utils/mfc/combobox_util.h"

///////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
#include "TMAlgoritmesCfg.h"
using namespace TMNodeDatabase;
using namespace TMCommunication;
///////////////////////////////////////////////////////////////////////

static
BOOL helper_AddToComboBox(
   CComboBox* pComboBox,
   int index,
   STRUCT_UNIT* pUnit)
{
	if (pComboBox && pUnit)
	{
		const int dataItem =(pUnit->bNodeLogicalAddress<<8)+pUnit->bUnitAddress;
	
		pComboBox->InsertString(index,pUnit->szUnitName);
		pComboBox->SetItemData(index,dataItem);		
		return TRUE;
	}
	return FALSE;
}

BOOL helper_GetAddress(
   CComboBox& refComboBox,
   BYTE* pbNodeAddress,
   BYTE* pbUnitAddress)
{
	using DUOTECNO::MFC_HELPER::CComboxbox_util;

	BOOL fResult = FALSE;
	DWORD_PTR data = 0;

	if(TRUE==CComboxbox_util::getComboBoxCurrSelDataItem(
				refComboBox,
				&data))
	{
		*pbNodeAddress = (data & 0xFF00) >> 8;
		*pbUnitAddress = (data & 0xFF);
		fResult = TRUE;
	}	
	return fResult;
}

///////////////////////////////////////////////////////////////////////

static
BOOL helper_update(
	CPropertyPageNodeSmapLite::CfgData_t* pDest,
	DUOTECNO::DATA::CSerSmapLiteEntry& refSrc)
{
	REQUIRE(pDest);

	// Lokale data updaten...		
	pDest->m_bExtSensNodeAddress=refSrc.m_bNodeAddress;
	pDest->m_bExtSensUnitAddress=refSrc.m_bUnitAddress;	

	if ((pDest->m_bExtSensNodeAddress!=0xFF)&&
		(pDest->m_bExtSensUnitAddress!=0xFF))
	{
		if (refSrc.m_szName[0]!=0)
		{
			pDest->m_szExtSensName = refSrc.m_szName;
		}
		pDest->m_fExtSensUseExternal=TRUE;
	}
	else
	{
		pDest->m_szExtSensName.Empty();
	}
	return TRUE;
}

static
BOOL helper_update(
	DUOTECNO::DATA::CSerSmapLiteEntry* pDest,
	CPropertyPageNodeSmapLite::CfgData_t& refSrc)
{
	REQUIRE(pDest);

	if(TRUE==refSrc.m_fExtSensUseExternal)
	{
		pDest->m_bNodeAddress=refSrc.m_bExtSensNodeAddress;
		pDest->m_bUnitAddress=refSrc.m_bExtSensUnitAddress;

		if (!refSrc.m_szExtSensName.IsEmpty())
		{
			if(refSrc.m_szExtSensName.GetLength()<sizeof(pDest->m_szName)-1)
			{
				strcpy(pDest->m_szName,refSrc.m_szExtSensName);
			}
		}
	}
	else
	{
		memset(pDest->m_szName,0,sizeof(pDest->m_szName));
		pDest->m_bNodeAddress=0xFF;
		pDest->m_bUnitAddress=0xFF;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////


// Moeten er ene aanmaken...

static
BOOL getDataSmapCfgClass( 
	CAppCfgSmapLiteClass** ppCfgClass)
{
	BOOL fResult=FALSE;
	CAppCfgSmapLiteInfo& refSmapInfo=CAppCfgSmapLiteInfo::getInstance();	
	CAppCfgSmapLiteFile& refSmapFile=refSmapInfo.getConfigFile();
	CAppCfgSmapLiteClass* const pClass = 
		dynamic_cast<CAppCfgSmapLiteClass*>(refSmapFile.GetClass
			(AppCfgSmapLiteClasses::ENUM_CONFIGCLASS_SMAPLITE));

	if (0!=pClass)
	{
		*ppCfgClass=pClass;
		fResult=TRUE;
	}
	return fResult;
}


static
BOOL getDataSmapLiteEntry(
	DUOTECNO::DATA::CSerSmapLiteEntry** ppData)
{
	BOOL fResult=FALSE;
	CAppCfgSmapLiteClass* pCfgClass = 0;

	if (TRUE==getDataSmapCfgClass(&pCfgClass))
	{	
		const int PAGE_NR = 0;
		const int BUTTON_NR = 0;
		DUOTECNO::DATA::CSerSmapLiteEntry* pTemp=0;

		if (TRUE==CAppCfgSmapLiteClass_Util::findEntry(
				*pCfgClass,
				PAGE_NR,
				BUTTON_NR,
				&pTemp))
		{
			*ppData=pTemp;
			fResult=TRUE;	
		}
	}
	return fResult;
}


static
BOOL createDataSmapLiteEntry(
	DUOTECNO::DATA::CSerSmapLiteEntry** ppData)
{
	BOOL fResult=FALSE;
	CAppCfgSmapLiteClass* pCfgClass = 0;

	if (TRUE==getDataSmapCfgClass(&pCfgClass))
	{	
		DUOTECNO::DATA::CSerSmapLiteEntry* pEntry=0;

		if (TRUE==CAppCfgSmapLiteClass_Util::createEntry(
				&pEntry))
		{
			const int PAGE_NR = 0;
			const int BUTTON_NR = 0;

			pEntry->m_pageNR = PAGE_NR;
			pEntry->m_buttonNR = BUTTON_NR;

			ASSERT(pCfgClass->GetMax()==0);
			fResult=CAppCfgSmapLiteClass_Util::addEntry(*pCfgClass,pEntry);
			if (TRUE==fResult)
			{
				*ppData=pEntry;
			}
			ASSERT(pCfgClass->GetMax()==1);
		}	
	}
	return fResult;
}

///////////////////////////////////////////////////////////////////////

// CPropertyPageNodeSmapLite dialog

IMPLEMENT_DYNAMIC(CPropertyPageNodeSmapLite, CPropertyPage)

CPropertyPageNodeSmapLite::CPropertyPageNodeSmapLite()
	: CPropertyPageNode(CPropertyPageNodeSmapLite::IDD)
{

}

CPropertyPageNodeSmapLite::CPropertyPageNodeSmapLite(CCanNode *pNode) :
CPropertyPageNode(CPropertyPageNodeSmapLite::IDD,pNode)
{

}


CPropertyPageNodeSmapLite::~CPropertyPageNodeSmapLite()
{
}

void CPropertyPageNodeSmapLite::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageNode::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_EXTERNAL_SENS, m_btnExtSens);
	DDX_Control(pDX, IDC_CHECK_EXTERNAL_SENS, m_btnCheckExtSens);
	DDX_Control(pDX, IDC_COMBO_EXTERNAL_SENS, m_cmbExtSens);
	DDX_Control(pDX, IDC_GROUP_EXTERNAL_SENS, m_groupExtSens);
	DDX_Control(pDX, IDC_LIST_STATUS, m_listStatus);
}


BEGIN_MESSAGE_MAP(CPropertyPageNodeSmapLite, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_EXTERNAL_SENS, &CPropertyPageNodeSmapLite::OnBnClickedBtnExternalSens)
	ON_BN_CLICKED(IDC_CHECK_EXTERNAL_SENS, &CPropertyPageNodeSmapLite::OnBnClickedCheckExternalSens)
	ON_CBN_SELENDOK(IDC_COMBO_EXTERNAL_SENS, &CPropertyPageNodeSmapLite::OnSelectExtSensor)
	ON_MESSAGE(WM_UPDATE_CONTROLS,&CPropertyPageNodeSmapLite::OnUpdateControls)
	ON_MESSAGE(WM_UPDATE_STATIC_CONTROLS,&CPropertyPageNodeSmapLite::OnUpdateStaticControls)
END_MESSAGE_MAP()


// CPropertyPageNodeSmapLite message handlers


// Oppassen:
// Indien er nog geen smap in de lijst aanwezig is dan wordt er ene aangemaakt 
// en toegevoegd aan de lijst.
// Altijd pageNR==0 && buttonNR==0
BOOL CPropertyPageNodeSmapLite::saveModified()
{
	BOOL fResult=FALSE;
	DUOTECNO::DATA::CSerSmapLiteEntry* pEntry=0;

	if(TRUE==getDataSmapLiteEntry(&pEntry)) { /* Absorbe */ }
	else if (TRUE==createDataSmapLiteEntry(&pEntry)) { /* Absorbe */ }

	ASSERT(0!=pEntry);

	if(0!=pEntry)
	{
		CAppCfgSmapLiteInfo& refSmapInfo=CAppCfgSmapLiteInfo::getInstance();		

		helper_update(pEntry,this->m_data);
		if (TRUE==refSmapInfo.save(m_pSelNode->getNodeAddress()))
		{		
			this->m_data.m_fChanged = FALSE;
			fResult=TRUE;
		}
		else
		{
			CString s;
			s.Format(_T("Error: Configuration file '%s' could not be opened for writing.\n"
				        "Please check if the file is not in use by another program or check the file settings."),
						refSmapInfo.GetFullPathName(refSmapInfo.getNodeAddress()));

			AfxMessageBox(s,MB_ICONSTOP | MB_OK);						 
		}
	}
	return fResult;
}


BOOL CPropertyPageNodeSmapLite::sendCfgData()
{
	BOOL fResult=FALSE;

	class CNotifier : public TMAlgoritmesCfg::INotifier
	{
	public:
		CNotifier(CPropertyPageNodeSmapLite& ref) : m_parent(ref) {};
		~CNotifier( ) { };

		virtual void AddMessage(const char* s)
		{
			m_parent.AddMessage(s);
		}
	private:
		CPropertyPageNodeSmapLite& m_parent;
	};

	if(TRUE==saveModified())
	{
		CNotifier notifier(*this);
		clearMessage( );

		TMAlgoritmesCfg::TMUploadSmapLiteCfg(m_pSelNode,notifier);
	}
	return fResult;
}

void CPropertyPageNodeSmapLite::updateControlsEnabled(void)
{
	switch(m_displayMode)
	{
		case DISPLAY_MODE_EXTSENS_NOT_SUPPORTED:
		{
			// disable alle controls.
			m_btnExtSens.ShowWindow(FALSE);
			m_btnCheckExtSens.ShowWindow(FALSE);
			m_cmbExtSens.ShowWindow(FALSE);
			m_groupExtSens.ShowWindow(FALSE);
		} break;

		// Enkel de checkbox enabled. (+send)
		case DISPLAY_MODE_EXTSENS_USE_INTERNAL_SENS:
		{
			m_btnExtSens.ShowWindow(TRUE);
			m_btnExtSens.EnableWindow(TMIsConnected() ? TRUE : FALSE);
			m_btnCheckExtSens.ShowWindow(TRUE);
			m_cmbExtSens.ShowWindow(FALSE);
			m_groupExtSens.ShowWindow(TRUE);
		} break;

		// Alle controls.
		case DISPLAY_MODE_EXTSENS_USE_EXTERNAL_SENS:
		{		
			m_btnExtSens.ShowWindow(TRUE);
			m_btnExtSens.EnableWindow(TMIsConnected() ? TRUE : FALSE);
			m_btnCheckExtSens.ShowWindow(TRUE);
			m_cmbExtSens.ShowWindow(TRUE);
			m_groupExtSens.ShowWindow(TRUE);
		} break;
	}
}

void CPropertyPageNodeSmapLite::OnBnClickedBtnExternalSens()
{
	if (TRUE==sendCfgData()) { /* absorbe */ }
	else { /* send cfg data mislukt. */ }
}

void CPropertyPageNodeSmapLite::OnBnClickedCheckExternalSens()
{
	const UINT state = m_btnCheckExtSens.GetState();

	if (state&BST_CHECKED)
    {
		if (this->m_data.m_fExtSensUseExternal!=TRUE)
		{
			m_displayMode=DISPLAY_MODE_EXTSENS_USE_EXTERNAL_SENS;
			this->m_data.m_fExtSensUseExternal=TRUE;
			this->m_data.m_fChanged=TRUE;
		}
    }
    else
    {
		if (this->m_data.m_fExtSensUseExternal!=FALSE)
		{
			m_displayMode=DISPLAY_MODE_EXTSENS_USE_INTERNAL_SENS;
			this->m_data.m_fExtSensUseExternal=FALSE;
			this->m_data.m_fChanged=TRUE;
		}
    }

	PostMessage(WM_UPDATE_CONTROLS,PARAM_UPDATE_CONTROLS);
}

void CPropertyPageNodeSmapLite::OnSelectExtSensor(void)
{
	PostMessage(WM_UPDATE_CONTROLS,PARAM_UPDATE_DATA);
}

// Return TRUE indien er units konden toegevoegd worden...
BOOL CPropertyPageNodeSmapLite::initComboBoxEntries()
{	
	CPtrList list;
	BOOL fResult=FALSE;
	CNodeDatabaseOperations nodeDbaseOperations(TMGetDocumentNodes());
	const int count=nodeDbaseOperations.SubClassUnits(UNIT_TYPE_SENS,&list);

	if (count>0)
	{
		// Add sensor units to the combo box...
		typedef TPtrListIterator<STRUCT_UNIT*> CPtrListUnit_it;
		int idx=0;
	
		for (CPtrListUnit_it iterator(list);
			 !iterator.IsDone( );
			 iterator.Next())
		{		
			STRUCT_UNIT* const pUnit = iterator.CurrentItem();

			if(0!=pUnit && 
			   pUnit->bNodeLogicalAddress!=m_pSelNode->getNodeAddress())
			{
				// Invoegen van de sensor units...
				// Item data = NodeAddress<<8 + unitAddress
				if (helper_AddToComboBox(&m_cmbExtSens,
										 idx++,
										 pUnit))
				{
					fResult=TRUE;
				}
			}
		}
	}
	return fResult;
}

BOOL CPropertyPageNodeSmapLite::getComboBoxData()
{
	// Oppassen:
	// Indien niet aktief is dan niet de data selectie aanpassen.
	using DUOTECNO::MFC_HELPER::CComboxbox_util;

	const UINT state = m_btnCheckExtSens.GetState();

	if (state&BST_CHECKED)
    {
		BYTE bNodeAddress = 0xFF;
		BYTE bUnitAddress = 0xFF;

		ASSERT(this->m_data.m_fExtSensUseExternal==TRUE);
	
		if (TRUE==helper_GetAddress(
					m_cmbExtSens,
					&bNodeAddress,
					&bUnitAddress))
		{
			if (this->m_data.m_bExtSensNodeAddress!=bNodeAddress)
			{
				this->m_data.m_bExtSensNodeAddress=bNodeAddress;
				this->m_data.m_fChanged=TRUE;
			}

			if (this->m_data.m_bExtSensUnitAddress!=bUnitAddress)
			{
				this->m_data.m_bExtSensUnitAddress=bUnitAddress;
				this->m_data.m_fChanged=TRUE;
			}

			if (TRUE==CComboxbox_util::getComboBoxStrCurrSel(
						m_cmbExtSens,
						this->m_data.m_szExtSensName))
			{
			
			}
		}   
    }
    else
    {
		//this->m_data.m_fExtSensUseExternal=FALSE;
    }
	return TRUE;
}

BOOL CPropertyPageNodeSmapLite::updateComboxBoxEntries()
{
	BOOL fResult=TRUE;

	if (this->m_data.m_fExtSensUseExternal==TRUE)		
	{
		// Zoeken naar de entry met node & unit adres...	
		
		if ((this->m_data.m_bExtSensNodeAddress!=0xFF)&&
			(this->m_data.m_bExtSensUnitAddress!=0xFF))
		{
			int i=0;
			const int count = m_cmbExtSens.GetCount();
			
			for(i=0;i<count;++i)
			{
				DWORD dwData = m_cmbExtSens.GetItemData(i);	
				if ((((dwData&0xFF00)>>8)==this->m_data.m_bExtSensNodeAddress)&&
					 (((dwData&0xFF)==this->m_data.m_bExtSensUnitAddress)))
				{
					m_cmbExtSens.SetCurSel(i);				
					break;
				}
			}

			if(i>=count)
			{
				// Node & unitadres niet gevonden in de combobox.

				CString s;
				if (!this->m_data.m_szExtSensName.IsEmpty())
				{
					s=this->m_data.m_szExtSensName;
				}
				m_cmbExtSens.SetWindowText(s); // _T("Error"));
				fResult=FALSE;	// Client vangt deze fout op.
			}
		}
		else
		{
			m_cmbExtSens.SetCurSel(0);		
			PostMessage(WM_UPDATE_CONTROLS,PARAM_UPDATE_DATA);
		}
	}
	else
	{
		m_cmbExtSens.SetCurSel(0);
	}
	return fResult;
}

BOOL CPropertyPageNodeSmapLite::updateCheckBox()
{
	m_btnCheckExtSens.SetCheck((this->m_data.m_fExtSensUseExternal==TRUE) ? 1 : 0);
	return this->m_data.m_fExtSensUseExternal;
}

void CPropertyPageNodeSmapLite::UpdateStaticControls()
{	
	using DUOTECNO::MFC_HELPER::CComboxbox_util;

	BOOL fSupportExternalSens = FALSE;

	ASSERT(0!=m_pSelNode);

	if (CNodeScreenmapLiteInfo_Util::isSupported(
			m_pSelNode,
			CNodeScreenmapLiteInfo_Util::SUPPORTED_FLAG_SENSOR))
	{
		fSupportExternalSens=TRUE;	
	}

	// Controleren of er items aanwezig zijn in de combobox.
	if (TRUE==fSupportExternalSens && TRUE==initComboBoxEntries())
	{
		CAppCfgSmapLiteInfo& refSmapInfo=CAppCfgSmapLiteInfo::getInstance();

		// Inladen van de configuratie.
		if (TRUE==refSmapInfo.load(m_pSelNode->getNodeAddress()))
		{						
			ASSERT(TRUE==refSmapInfo.isLoaded());	

			DUOTECNO::DATA::CSerSmapLiteEntry* pEntry=0;

			if(TRUE==getDataSmapLiteEntry(&pEntry))
			{
				// Lokale data updaten...		
				helper_update(&this->m_data,*pEntry);
			}
		}
		else
		{	
			// Oppassen:
			// Indien het inladen van het bestand mislukt omdat het niet aanwezig is 
			// dan zijn er geen entries in het errors array.
			// Indien er verkeerde velden zijn dan worden deze wel in het errors array
			// gestopt. 
			// Er wordt enkel een melding getoond indien er fouten in het array aanwezig
			// zijn... Dus NIET wanneer het bestand niet kon ingeladen worden.

			CAppCfgSmapLiteFile& refSmapFile = refSmapInfo.getConfigFile( );
			const int nrOfErrors= refSmapFile.GetNrErrors();
			if (nrOfErrors>0)
			{
				int i;
				CString szTemp;
				CString szErrors;
				CString szMessage;

				for (i=0;i<nrOfErrors;++i)
				{
					szTemp=refSmapFile.GetErrors(i) + "\r\n";
					szErrors+=szTemp;
				}

				szMessage.Format(_T("Error: Failed to load the configuration file:'%s'\n"
								    "These errors happened:\n%s"),
									refSmapInfo.GetFullPathName(refSmapInfo.getNodeAddress()),
								    szErrors);

				AfxMessageBox(szMessage,MB_OK|MB_ICONSTOP);
			}
		}

		if(FALSE==updateComboxBoxEntries())
		{
			// Er is een external sensor gekoppeld die niet in de nodedatabase
			// aanwezig is.

			CString s;
			
			s.Format(_T("Sensor '%s' with address 0x%02x,0x%02x could not be found in the database.\n"
						"Would you like to remove this configuration ?\n"),
						this->m_data.m_szExtSensName,
						this->m_data.m_bExtSensNodeAddress,
						this->m_data.m_bExtSensUnitAddress);

			if (IDYES==AfxMessageBox(s,MB_YESNO))
			{
				this->m_data.m_bExtSensNodeAddress=0xFF;
				this->m_data.m_bExtSensUnitAddress=0xFF;
				this->m_data.m_fExtSensUseExternal=FALSE;
				this->m_data.m_fChanged=TRUE;

				// Opnieuw aanroepen...
				updateComboxBoxEntries();				
			}			

			PostMessage(WM_UPDATE_CONTROLS,PARAM_UPDATE_CONTROLS);
		}

		updateCheckBox();

		if (this->m_data.m_fExtSensUseExternal==TRUE)
		{
			m_displayMode=DISPLAY_MODE_EXTSENS_USE_EXTERNAL_SENS;	
		}
		else
		{
			m_displayMode=DISPLAY_MODE_EXTSENS_USE_INTERNAL_SENS;
		}
	}
	else
	{
		m_displayMode=DISPLAY_MODE_EXTSENS_NOT_SUPPORTED;
	}

	// Speciale code die ervoor zorgt dat de pijlen op/neer
	// zichtbaar zijn voor de combobox.
	CComboxbox_util::setComboBoxSetHeight(&m_cmbExtSens);
}

BOOL CPropertyPageNodeSmapLite::OnInitDialog()
{
	CPropertyPageNode::OnInitDialog();

	// Oppasssen:
	// Initialisatie gebeurd in CPropertyPageNodeSmapLite::OnSetActive()

	clearMessage( );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPropertyPageNodeSmapLite::initData(CfgData_t* pData)
{
	if(0!=pData)
	{
		pData->m_szExtSensName.Empty();
		pData->m_bExtSensNodeAddress=0xFF;
		pData->m_bExtSensUnitAddress=0xFF;
		pData->m_fExtSensUseExternal=FALSE;
		pData->m_fChanged=FALSE;
	}
}


LONG CPropertyPageNodeSmapLite::OnUpdateControls(WPARAM wParam,LPARAM lParam)
{
	if (PARAM_UPDATE_CONTROLS==wParam)
	{
		// Update Controls...
		updateCheckBox();
		updateComboxBoxEntries();	

		updateControlsEnabled();		// nieuw
	}
	else if (PARAM_UPDATE_DATA==wParam)
	{
		// Update data...
		getComboBoxData();
	}
	else
	{
		ASSERT(0);
	}
	return TRUE;
}

BOOL CPropertyPageNodeSmapLite::OnKillActive()
{
	CheckSaveModified( );
	return CPropertyPageNode::OnKillActive();
}

void CPropertyPageNodeSmapLite::PostNcDestroy()
{
	CheckSaveModified( );
	CPropertyPageNode::PostNcDestroy();
}

BOOL CPropertyPageNodeSmapLite::CheckSaveModified()
{
	BOOL fResult=TRUE;

	if (TRUE==this->m_data.m_fChanged)
	{	
		CString s;
	
		if (TMIsConnected())
		{
			s.Format(_T("Changes have been made to external sensor screenmapping.\n"						
						 "Would you like to send this configuration changes ?"));
		}
		else
		{
			s.Format(_T("Changes have been made to external sensor screenmapping.\n"
						 "These changes have not been send to the node.\n"
						 "Would you like to save these changes ?"));
		}

		if (IDYES==AfxMessageBox(s,MB_YESNO))
		{		
			if (TMIsConnected())
			{
				if (TRUE==sendCfgData()) { /* absorbe */ }
			}			
			else
			{
				if (TRUE==saveModified()) { /* absorbe */ }			
			}
		}
	}
	return fResult;
}

//Initialisatie van lokale cfg data + Controls updaten mbv ingeladen data.
BOOL CPropertyPageNodeSmapLite::OnSetActive()
{
	m_displayMode=DISPLAY_MODE_EXTSENS_NOT_SUPPORTED;
	initData(&this->m_data);
	PostMessage(WM_UPDATE_STATIC_CONTROLS);
	PostMessage(WM_UPDATE_CONTROLS,PARAM_UPDATE_CONTROLS);

	return CPropertyPageNode::OnSetActive();
}

LONG CPropertyPageNodeSmapLite::OnUpdateStaticControls(WPARAM wParam,LPARAM lParam)
{
	UpdateStaticControls();
	return TRUE;
}

void CPropertyPageNodeSmapLite::AddMessage(const char* s)
{
	m_listStatus.InsertString(m_stringIndex++,s);	
}

void CPropertyPageNodeSmapLite::clearMessage(void)
{
	m_stringIndex=0;
	m_listStatus.ResetContent();
}