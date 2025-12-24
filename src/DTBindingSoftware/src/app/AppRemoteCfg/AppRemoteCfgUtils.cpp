#include "stdafx.h"
#include "AppRemoteCfg.h"		// Header bestand voor AppRemoteCfgUtils - friend class.
#include "Mainfrm.h"			// nodig voor het opslaan : Evt via een sendMessage ??

#if (USE_TOOLS_APPREMOTECFG == 1)

//////////////////////////////////////////////////////////////////////
#include "TMGlobals.h"
using namespace TMGlobalSettings;		// Filesettings: updateProjectFileInfo
using namespace TMBindingDatabase;		// Update van de binding database
using namespace TMApplicationViews;		// Update van de views
using namespace TMNodeDatabase;			// de node database...
//////////////////////////////////////////////////////////////////////
#define DEBUG_LOCAL_ON	1
//////////////////////////////////////////////////////////////////////
#if (DEBUG_LOCAL_ON == 1)
	#define TRACER_1(sz,p1)					 TRACE(_T(sz), p1 )
	#define TRACER_4(sz,p1,p2,p3,p4)		 TRACE(_T(sz), p1, p2, p3, p4)
#else
	#define TRACER_1(sz,p1)
	#define TRACER_4(sz,p1,p2,p3,p4)
#endif 
//////////////////////////////////////////////////////////////////////
namespace DUOTECNO {
//////////////////////////////////////////////////////////////////////

static void updateNode(CCanNode* pNode, int nrOfEntries, int xorChecksum, bool clearModified) {
	if (NULL == pNode) {
		return;
	}
	if (nrOfEntries>=0) {
		pNode->SetNrEntries(nrOfEntries);
	}
	if (xorChecksum>=0) {
		pNode->SetXORChecksum(xorChecksum);
	}			
	if(clearModified) {
		CBindingFile* const pBindingFile = pNode->GetBindingFilePtr();
		if (NULL != pBindingFile) {
			pBindingFile->ClearModified();
		}
	}
}

static void updateNodeDatabase(CNodeDatabase* pNodeDatabase, BYTE bNodeAddress, int nrOfEntries, int checkSum, bool clearModified) {
	if (NULL == pNodeDatabase) {
		return;
	}
	CCanNode* pNode = NULL;
	int dummy=-1;
	if (pNodeDatabase->FindNode(bNodeAddress,&dummy,&pNode)) {
		updateNode(pNode,nrOfEntries,checkSum,clearModified);
	}	
}


// Mogen enkel de bindingfile properties van een node updaten wanneer het 
// aantal bindingEntries & xor checksum een waarde >= 0 hebben.
// Indien de waarden '-1' dan werd de bindingfile niet geopend.
static void updateNodeBindingFile(AppCfgBindInfoNode& refNodeInfo,bool bindingsWritten) {
		
	const BindingFileFsmStatus_t status = refNodeInfo.getStatus();	
	const int nrOfEntries = refNodeInfo.getBindFileNrOfEntries( );
	const int xorChecksum = refNodeInfo.getBindFileChecksum( );

	ASSERT(refNodeInfo.getNodeAddress()>=0x03 && refNodeInfo.getNodeAddress()<=0xFF);
	const BYTE bNodeAddress = refNodeInfo.getNodeAddress();		

	if (nrOfEntries>=0 && xorChecksum>=0) {

		bool clearModified = false;		// Enkel wissen van de modified vlag wanneer ok doorgestuurd werd.
		if (bindingsWritten && BINDINGFILE_FSM_STATUS_DONE == status) {
			clearModified = true;	
		}

		ASSERT(nrOfEntries>=0 && nrOfEntries<=0xFF);
		ASSERT(xorChecksum>=0 && xorChecksum<=0xFF);
		
		// Updaten voor zowel de runtime & document nodedatabase.		
		updateNodeDatabase(TMGetRuntimeNodes(),bNodeAddress, nrOfEntries, xorChecksum, clearModified);
		updateNodeDatabase(TMGetDocumentNodes(),bNodeAddress, nrOfEntries, xorChecksum, clearModified);

		TRACER_4("Update node 0x%02x bindingfile: nrOfEntries=%d, Xor=0x%02x, clearModified= %d\r\n",
			    (int) bNodeAddress, nrOfEntries, xorChecksum, clearModified);
	}
	else {
		TRACER_1("Update node 0x%02x bindingfile: NOT UPDATING BECAUSE FILE WAS NOT CHANGED",(int) bNodeAddress);
	}
}

// parem 'bindingswritte': Indien true dan wordt de bindingfile modified vlag gewist wanneer 
// alles correct doorgestuurd werd.
static void updateNodeBindingFiles(AppCfgBindInfoNodes& refNodes,bool bindingsWritten) {		
	const int count = refNodes.getCount();
	for (int i=0;i<count;i++) {
		AppCfgBindInfoNode* pInfo = NULL;
		if (refNodes.getNodeInfo(i,&pInfo)) {
			updateNodeBindingFile(*pInfo,bindingsWritten);
		}
	}
}

// Alle binding file entries als failed zetten die niet konden doorgestuurd worden...
static void updateBindingEntriesFailed(AppCfgBindInfoNodes& refNodes) 
{
	const int count = refNodes.getCount();
	for (int i=0;i<count;i++) {
		AppCfgBindInfoNode* pInfo = NULL;
		if (refNodes.getNodeInfo(i,&pInfo)) {
			const BindingFileFsmStatus_t status = pInfo->getStatus();	
			if (BINDINGFILE_FSM_STATUS_ERROR == status || BINDINGFILE_FSM_STATUS_CANCELLED == status) {
				TMGetBindingApp()->SetAllBindingsSendFailed(pInfo->getNodeAddress());
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////

bool AppRemoteCfgUtils::updateProjectFileInfo(AppRemoteCfg& refParent) {
	if (NULL == refParent.m_pAppRemoteCfg) {
		ASSERT(NULL != refParent.m_pAppRemoteCfg);
		return false;
	}	
	const CString szPathName(TMGetFileSettings()->GetPathName());	
	const CString fileName(TMGetFileSettings()->GetTitle());	
	refParent.m_pAppRemoteCfg->setProjectInfo((LPCSTR)szPathName, (LPCSTR) fileName);
	return true;
}


// Voor het versturen van de bindings moeten de binding bestanden gesaved worden.
bool AppRemoteCfgUtils::saveModifiedProject(AppRemoteCfg& refParent) {
	CMainFrame *const pMainWnd = ((CMainFrame*) AfxGetMainWnd());
	if (NULL != pMainWnd) {
		CDocument* const pDoc = pMainWnd->GetActiveDocument();
		if ((NULL != pDoc) && (pDoc->IsModified())) {
			pDoc->DoFileSave();
			return true;
		}		
	}
	return false;
}

// update bindings write.
// Zie de code: CMainFrame::OnBindingsNodeFSMWriteDone
bool AppRemoteCfgUtils::updateBindingsWritten(AppRemoteCfg& refParent) {
	HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));	

	// Updaten van de binding file properties van de nodes: nrofentries, xor, modified,...
	updateNodeBindingFiles(refParent.m_bindInfoNodes,true);	

	// Updaten van de views + binding entries database
	TMGetBindingApp()->SetAllSend();	
	TMGetViews()->OnBindingdatabaseUpdate();
	SetCursor(oldCursur);
	return true;
}

// update bindings erased.
bool AppRemoteCfgUtils::updateBindingsErased(AppRemoteCfg& refParent) {
	HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));	

	// Updaten van de binding file properties van de nodes: nrofentries, xor, modified,...
	updateNodeBindingFiles(refParent.m_bindInfoNodes,false);	

	// Updaten van de views + binding entries database
	TMGetBindingApp()->SetAllErased();		
	TMGetViews()->OnBindingdatabaseUpdate();
	SetCursor(oldCursur);
	return true;
}

bool AppRemoteCfgUtils::updateBindingsWrittenFailed(AppRemoteCfg& refParent) {
	HCURSOR oldCursur = SetCursor(LoadCursor(NULL, IDC_WAIT));	

	// Updaten van de binding file properties van de nodes: nrofentries, xor, modified,...
	updateNodeBindingFiles(refParent.m_bindInfoNodes,true);	

	// Updaten van de views + binding entries database
	updateBindingEntriesFailed(refParent.m_bindInfoNodes);
	TMGetViews()->OnBindingdatabaseUpdate();
	SetCursor(oldCursur);
	return true;
}

// Gebruikt de runtime nodedatabase (idem zoals de BindingFSM)...
bool AppRemoteCfgUtils::getNodeBindingFiles(AppRemoteCfg& refParent,std::list<int>* nodes, bool onlyModified) {
	CNodeDatabase* const pNodeDatabase = TMGetRuntimeNodes();
	if (NULL == pNodeDatabase) {
		return false;
	}
	int nrOfNodes = 0;
	pNodeDatabase->NROfNodes(&nrOfNodes);
	if (0 == nrOfNodes) {
		return false;
	}

	nodes->clear();

	for (int i=0;i<nrOfNodes;++i) {
		CCanNode* pNode = NULL;
		if (pNodeDatabase->GetNode(i,&pNode)) {
			CBindingFile* const pBindingFile = pNode->GetBindingFilePtr();
			if (NULL != pBindingFile) {
				BYTE bNodeAddress = 0xFF;
				if (onlyModified) {
					if (pBindingFile->IsModified()) {
						bNodeAddress = pNode->getNodeAddress();
					}
				}
				else {
					bNodeAddress = pNode->getNodeAddress();
				}
				if (bNodeAddress<0xFD) {
					nodes->push_back(bNodeAddress);
				}
			}
		}
	}
	return (!nodes->empty());
}


bool AppRemoteCfgUtils::getListNames(AppRemoteCfg& refParent,AppCfgRemoteActionList* pListDest)
{
	CNodeDatabase* const pNodeDatabase = TMGetDocumentNodes();
	if (NULL == pNodeDatabase) {
		return false;
	}
	int nrOfNodes = 0;
	pNodeDatabase->NROfNodes(&nrOfNodes);
	if (0 == nrOfNodes) {
		return false;
	}

	for (int i=0;i<nrOfNodes;++i) {
		CCanNode* pNode = NULL;
		if (pNodeDatabase->GetNode(i,&pNode)) {
			pListDest->add(new AppCfgRemoteActionWriteName(pNode->getNodeAddress(), -1, pNode->getNodeName()));
			const int numberOfUnits = pNode->getNROfUnits();
			for (int j=0; j<numberOfUnits; ++j) {
				STRUCT_UNIT* pUnit = NULL;
				if (pNodeDatabase->GetUnit(pNode,j,&pUnit)) {
					pListDest->add(new AppCfgRemoteActionWriteName(pNode->getNodeAddress(), pUnit->bUnitAddress, pUnit->szUnitName));		
				}
			}
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////
}; /* namespace DUOTECNO */
//////////////////////////////////////////////////////////////////////
#endif