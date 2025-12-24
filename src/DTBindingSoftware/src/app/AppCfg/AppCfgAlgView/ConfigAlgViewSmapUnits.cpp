#include "stdafx.h"
#include "ConfigAlgViewsmapunits.h"

#include "nodescreenmapinfo.h"						
#include "app\appipc\appipccfgtool\ipluginconfigtoolapp.h"	
#include "TMAlgoritmesCfg.h"
#include "cannode.h"

#include "NodeLCDScreenmapFSMInfo.h"
#include "nodelcdscreenmaps_util.h"
#include "screenmaps.h"
#include "screenmapid.h"
#include "NodeDatabaseOperations.h"

/////////////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalHSM;
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
/////////////////////////////////////////////////////////////////////////////

CConfigAlgViewSmapUnits::CConfigAlgViewSmapUnits(CCanNode* pNode,IViewDisplay* pNotifyView) :
Base(pNode,pNotifyView), m_useOnlyDuotecnoProApp(FALSE) 
{ 
	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
	if (CNodeScreenmapInfo_Util::isOnlyProAppConfig(pNode)) {
		m_useOnlyDuotecnoProApp = TRUE;
	}
}

CConfigAlgViewSmapUnits::~CConfigAlgViewSmapUnits(void)
{
}

BOOL CConfigAlgViewSmapUnits::Load(void) {
	return FALSE;
}


#if(USE_SMAP_ENERGYMEASURE == 1)

// Aanpassingen V16.21: Screenmapping van de virtuele energie meet units.
// Gebruik van een CNodeLCDScreenmapFSMInfo_Util klasse.
BOOL CConfigAlgViewSmapUnits::Upload(void) {
	BOOL result = FALSE;
	CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
	if (pRefScreenmapsUpdateInfo) {
		if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {
			if (AfxGetMainWnd() != NULL) {
				DisplayMessage("Upload screenmaps started...");

				CString strError;
				pRefScreenmapsUpdateInfo->Clear();			
				if (!CNodeLCDScreenmapFSMInfo_Util::addNode(pRefScreenmapsUpdateInfo, &getCanNode(), CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS, &strError)) {
					DisplayMessage(strError);
					return FALSE;
				}
				 				 
				TMGetGlobalHSM()->EvOperationUploadScreenmaps();
				result = TRUE;
			}
		}
		else {
			CString szError("Screenmaps FSM is locked");
			DisplayMessage(szError);
		}
	}
	return result;
}

// Aanpassingen V16.21: Screenmapping van de virtuele energie meet units.
// Gebruik van een CNodeLCDScreenmapFSMInfo_Util klasse.
BOOL CConfigAlgViewSmapUnits::UploadToAll(void) {
	BOOL result = FALSE;
	CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
	if (pRefScreenmapsUpdateInfo) {
		if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {							
			CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());
			CPtrList List;
			const int nCount = NodeDatabaseOperations.SubclassNodes(NODE_LCD,&List);
			if (nCount != 0) {
				if (AfxGetMainWnd() != NULL) {
					DisplayMessage("Upload screenmaps started...");
					pRefScreenmapsUpdateInfo->Clear();			

					POSITION pos = List.GetHeadPosition();
					while(pos!=NULL) {
						CCanNode *pNode = 0;
						if ((pNode = (CCanNode*) List.GetNext(pos)) != NULL) {
							if (CNodeScreenmapInfo_Util::isSupported(pNode)) {
								CString strError;
								if (!CNodeLCDScreenmapFSMInfo_Util::addNode(pRefScreenmapsUpdateInfo, pNode, CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS, &strError)) {
									DisplayMessage(strError);
									return FALSE;
								}													
							}
						}
					}										
					TMGetGlobalHSM()->EvOperationUploadScreenmaps();
					result = TRUE;
				}
			}			
		}
		else {
			CString szError("Screenmaps FSM is locked");
			DisplayMessage(szError);
		}
	}
	return result;
}


#else

BOOL CConfigAlgViewSmapUnits::Upload(void) {
	BOOL result = FALSE;
	CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
	if (pRefScreenmapsUpdateInfo) {
		if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {
			if (AfxGetMainWnd() != NULL) {
				DisplayMessage("Upload screenmaps started...");
				pRefScreenmapsUpdateInfo->Clear();
				pRefScreenmapsUpdateInfo->AddNodeToSmapFsm(&getCanNode());							
				// Sedert v0571: Om te bepalen welke screenmaps er moeten doorgestuurd worden.
				pRefScreenmapsUpdateInfo->setType(CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS );
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_DIMMER));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_SWITCH));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_CONTROLS));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_SENS));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_DUOSWITCH));
				pRefScreenmapsUpdateInfo->addScreenmapID(CScreenmapID(SCREENMAP_AUDIO));

				TMGetGlobalHSM()->EvOperationUploadScreenmaps();
				result = TRUE;
			}
		}
		else {
			CString szError("Screenmaps FSM is locked");
			DisplayMessage(szError);
		}
	}
	return result;
}

BOOL CConfigAlgViewSmapUnits::UploadToAll(void) {
	BOOL result = FALSE;
	CNodeLCDScreenmapFSMInfo* const pRefScreenmapsUpdateInfo = CNodeLCDScreenmapFSMInfo::GetInstance();		
	if (pRefScreenmapsUpdateInfo) {
		if (pRefScreenmapsUpdateInfo->IsLocked() == FALSE) {							
			CNodeDatabaseOperations NodeDatabaseOperations(TMGetDocumentNodes());
			CPtrList List;
			const int nCount = NodeDatabaseOperations.SubclassNodes(NODE_LCD,&List);
			if (nCount != 0) {
				if (AfxGetMainWnd() != NULL) {
					DisplayMessage("Upload screenmaps started...");
					pRefScreenmapsUpdateInfo->Clear();
					POSITION pos = List.GetHeadPosition();
					while(pos!=NULL) {
						CCanNode *pNode = 0;
						if ((pNode = (CCanNode*) List.GetNext(pos)) != NULL) {
							if (CNodeScreenmapInfo_Util::isSupported(pNode)) {
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
					result = TRUE;
				}
			}			
		}
		else {
			CString szError("Screenmaps FSM is locked");
			DisplayMessage(szError);
		}
	}
	return result;
}

#endif 


int CConfigAlgViewSmapUnits::GetViewID(void) const
{
	using namespace DUOTECNO::PLUGIN;
	return VIEW_SMAP_UNITS;
}

BOOL CConfigAlgViewSmapUnits::IsSupported(void)
{
	const BOOL fShowWindow = CNodeScreenmapInfo_Util::isSupported(&getCanNode());

	return fShowWindow;
}

BOOL CConfigAlgViewSmapUnits::IsLoadSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapUnits::IsFileFound(void)
{
	// TODO - Implementeren.

	return TRUE;
}

BOOL CConfigAlgViewSmapUnits::IsUploadCanSupported(void)
{
	return !m_useOnlyDuotecnoProApp;	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

BOOL CConfigAlgViewSmapUnits::IsDownloadCanSupported(void)
{
	return FALSE;
}

BOOL CConfigAlgViewSmapUnits::IsUploadCanAllSupported(void)
{
	return !m_useOnlyDuotecnoProApp;	// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
}

const char* CConfigAlgViewSmapUnits::getName(void)
{
	return "Screenmap units";
}

BOOL CConfigAlgViewSmapUnits::IsUseConfigurator(void)
{
	// Configuratie via de configurator
	return TRUE;
}

BOOL CConfigAlgViewSmapUnits::getFullPathName(int idx,CString* pszFullPathName)
{
	// TODO: Momenteel niet ondersteund.
	return FALSE;
}

BOOL CConfigAlgViewSmapUnits::IsUseFTPProtocol(void)
{
	// Gebruikt External config commandos.
	return FALSE;
}

// Sedert V16.01: CHANGES_SMAP_ONLY_FOR_APP
BOOL CConfigAlgViewSmapUnits::getInfoMessages(CStringArray* messages) {
	if (!m_useOnlyDuotecnoProApp) {
		ASSERT(0 != messages);
		return FALSE;
	}
	return CConfigAlgView_Util::getInfoStrings(messages, CConfigAlgView_Util::STRINGS_ID_SCREENMAP_PROAPP);
}