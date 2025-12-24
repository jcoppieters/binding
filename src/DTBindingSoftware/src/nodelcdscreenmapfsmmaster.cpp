// ScreenmapsUpdateLCD.cpp: implementation of the CScreenmapsUpdateLCD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "nodelcdscreenmapfsmmaster.h"
#include "NodeLCDScreenmapFSM.h"
#include "NodeLcdScreenmaps.h"
#include "NodeLCDScreenmaps_Util.h"
#include "CanNode.h"
#include "ComApplication.h"
#include "MainFrm.h"
#include "NodeLCDScreenmapFSMInfo.h"
#include "FactoryNodeLCDScreenmaps.h"
#include "App\AppCfg\AppCfgsmap\configsmapsmapinfo.h"
#include "App\AppCfg\AppCfgsmap\configsmapsmapinfofile.h"
#include "App\AppCfg\AppCfgsmap\configsmapsmapinfoclass.h"
#include "screenmaps.h"
// -----------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
using namespace TMGlobalSettings;
// ------------------------------------


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNodeLCDScreenmapFSMMaster::CNodeLCDScreenmapFSMMaster(IComApplication *pCom) :
m_pComApplication(pCom), m_currentState( STATE_SCREENMAP_NODE_IDLE ), m_pCurrNodeLCDScreenmaps( 0 ),
m_pChildFSM( 0 ), m_iCurrentNode( 0 ) { }

CNodeLCDScreenmapFSMMaster::~CNodeLCDScreenmapFSMMaster() {
	if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {
		CNodeLCDScreenmapFSMInfo::GetInstance()->SetLock(FALSE);
	}
	DestroyScreenmap();
	DestroyChild();
}

//////////////////////////////////////////////////////////////////////
BOOL CNodeLCDScreenmapFSMMaster::CreateNewLCDScreenmap(CCanNode *pNode,CNodeLcdScreenmaps** pReturn)
{
	if (TMGetFileSettings() != NULL) {
		CString szPathName(TMGetFileSettings()->GetFullyQualifiedPathName());		

		if (szPathName.IsEmpty() == FALSE) {

			DestroyScreenmap();

			const CNodeLCDScreenmapFSMInfo::Type type = CNodeLCDScreenmapFSMInfo::GetInstance()->getType( );
			CFactoryNodeLCDScreenmaps::Type factoryType =  CFactoryNodeLCDScreenmaps::TYPE_SCREENMAP_UPDATE_UNITS;

			switch (type) {
				case CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_UNITS: {
					factoryType = CFactoryNodeLCDScreenmaps::TYPE_SCREENMAP_UPDATE_UNITS; 
				} break;
				case CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MYHOME: {
					factoryType = CFactoryNodeLCDScreenmaps::TYPE_SCREENMAP_UPDATE_MYHOME; 					
				} break;
				case CNodeLCDScreenmapFSMInfo::TYPE_SCREENMAP_UPDATE_MAIN: {
					factoryType = CFactoryNodeLCDScreenmaps::TYPE_SCREENMAP_UPDATE_MAIN; 					
				} break;
			}				

			CNodeLcdScreenmaps* const pNew = CFactoryNodeLCDScreenmaps::create(TMGetRuntimeNodes(), pNode, factoryType);
			if (pNew != NULL) {			
				pNew->Load(szPathName);

				// Screenmap default start pagina nummer die ingeladen werden door de 
				// configuratie bestanden copieren naar de screenmap info structuren.

				CConfigSmapSmapInfo& refConfigSmapInfo = CConfigSmapSmapInfo::getInstance( );
				if (refConfigSmapInfo.load(pNode->getNodeAddress( ))) {
					CNodeLcdScreenmaps_Util::update_StartPage(refConfigSmapInfo,pNew);
				}

				*pReturn = pNew;
				return TRUE;
			}					
		}		
	}
	return FALSE;
}


BOOL CNodeLCDScreenmapFSMMaster::GetNextLCDCanNode(CCanNode **pReturnNode) {
	CNodeLCDScreenmapFSMInfo* const ptr = CNodeLCDScreenmapFSMInfo::GetInstance();
	if (ptr) {
		const int nCount = ptr->Count();
		if (m_iCurrentNode < nCount) {
			CCanNode* const pNode = ptr->GetSmapCurrNode(m_iCurrentNode);			
			if (pNode != NULL) {
				*pReturnNode = pNode;
				m_iCurrentNode++;
				return TRUE;				
			}
		}	
	}
	*pReturnNode = NULL;
	return FALSE;
}


void CNodeLCDScreenmapFSMMaster::Trigger(ENUM_EVENT_SCREENMAP_NODE ev) {
	if (m_pComApplication != NULL) {
		switch(ev) {
			case FSM_EVENT_SIGNAL_NODE_ERROR: {
				m_pComApplication->Trigger( WM_SCREENMAP_FSM_TRIGGER_ERROR );			
			} break;
			case FSM_EVENT_SIGNAL_NODE_CANCEL: {
				m_pComApplication->Trigger( WM_SCREENMAP_FSM_TRIGGER_CANCEL );
			} break;
			case FSM_EVENT_SIGNAL_NODE_DONE : {
				m_pComApplication->Trigger( WM_SCREENMAP_FSM_TRIGGER_OK );
			} break;
			default: {
				ASSERT(0);
			} break;
		}
	}
}

void CNodeLCDScreenmapFSMMaster::DestroyChild() {
	if (m_pChildFSM != NULL) {
		delete m_pChildFSM;
		m_pChildFSM = NULL;
	}
}

void CNodeLCDScreenmapFSMMaster::DestroyScreenmap() {
	if (m_pCurrNodeLCDScreenmaps != NULL) {
		delete m_pCurrNodeLCDScreenmaps;
		m_pCurrNodeLCDScreenmaps = NULL;
	}
}


// Aanpassingen V16.21: Aanpassingen voor het doorsturen van de smap voor de energie meet units.
// De 'CScreenmapIDListIterator' instantie wordt hier aangemaakt ifv de firmware versie van de node.
// Deze instantie wordt doorgegeven aan de child fsm.
void CNodeLCDScreenmapFSMMaster::FSM(ENUM_EVENT_SCREENMAP_NODE e)
{
	switch (m_currentState) {
		case STATE_SCREENMAP_NODE_IDLE: {
			switch (e) {
				case FSM_EVENT_OPERATION_NODE_START : {			
					m_iCurrentNode = 0;							
					DestroyChild();

					CCanNode *pNode = NULL;
					if (GetNextLCDCanNode(&pNode) == TRUE) {
						if (CreateNewLCDScreenmap(pNode,&m_pCurrNodeLCDScreenmaps) == TRUE) {

#if(USE_SMAP_ENERGYMEASURE == 1)
							ASSERT(NULL != pNode);

							const CScreenmapIDListIterator smapIterator = CNodeLCDScreenmapFSMInfo_Util::createSmapIterator(CNodeLCDScreenmapFSMInfo::GetInstance( ), pNode->getNodeAddress());
							if (smapIterator.IsEmpty()) {
								ASSERT(0);
								Fire_OnScreenmapFSMError();
								return;
							}

#else
							const CScreenmapIDListIterator smapIterator = CNodeLCDScreenmapFSMInfo::GetInstance( )->createScreenmapIDIterator( );
#endif
							if ((m_pChildFSM = new CNodeLCDScreenmapFSM((INodeLCDScreenmapFSMMaster*) this, m_pCurrNodeLCDScreenmaps, smapIterator)) != NULL) {			
								m_pChildFSM->UpdateLCD();
								m_currentState = STATE_SCREENMAP_NODE_BUSY;
							}							
							else {
								ASSERT(0);
								Fire_OnScreenmapFSMError();
							}								
						}							
						else {									
							ASSERT(0);
							Fire_OnScreenmapFSMError();
						}
					}
					else {							
						ASSERT(0);
						Fire_OnScreenmapFSMError();
					}
				} break;
				default: {					
					ASSERT(0);
				} break;			
			} // switch(e) 
		} break; //case STATE_SCREENMAP_NODE_IDLE:

		case STATE_SCREENMAP_NODE_BUSY : {
			switch(e) {
				case FSM_EVENT_SIGNAL_NODE_ERROR: {	
					// Error received 
					DestroyChild();
					// Fill in Structure !!!
					DestroyScreenmap();													
					Fire_OnScreenmapFSMError();
					m_currentState = STATE_SCREENMAP_NODE_IDLE;
				} break;

				case FSM_EVENT_SIGNAL_NODE_DONE: {
					if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {
						CNodeLCDScreenmapFSMInfo::GetInstance()->SetSendCounter(m_iCurrentNode);
					}
					// Error received 
					DestroyChild();
					// Fill in Structure !!!
					DestroyScreenmap();

					CCanNode *pNode;
					if (GetNextLCDCanNode(&pNode) == TRUE) {
						if (CreateNewLCDScreenmap(pNode,&m_pCurrNodeLCDScreenmaps) == TRUE) {

#if(USE_SMAP_ENERGYMEASURE == 1)
							ASSERT(NULL != pNode);

							const CScreenmapIDListIterator smapIterator = CNodeLCDScreenmapFSMInfo_Util::createSmapIterator(CNodeLCDScreenmapFSMInfo::GetInstance( ), pNode->getNodeAddress());
							if (smapIterator.IsEmpty()) {
								ASSERT(0);
								Fire_OnScreenmapFSMError();
								return;
							}							
#else
							const CScreenmapIDListIterator smapIterator = CNodeLCDScreenmapFSMInfo::GetInstance( )->createScreenmapIDIterator( );
#endif 
							if ((m_pChildFSM = new CNodeLCDScreenmapFSM((INodeLCDScreenmapFSMMaster*) this, m_pCurrNodeLCDScreenmaps, smapIterator)) != NULL) {			
								m_pChildFSM->UpdateLCD();
								m_currentState = STATE_SCREENMAP_NODE_BUSY;							
							}							
							else {
								ASSERT(0);
								Fire_OnScreenmapFSMError();
							}								
						}							
						else {									
							ASSERT(0);
							Fire_OnScreenmapFSMError();
						}
					}
					else {							
						Fire_OnScreenmapFSMDone();
						m_currentState = STATE_SCREENMAP_NODE_IDLE;
					}																								
				} break;

				case FSM_EVENT_SIGNAL_NODE_CANCEL: {					
					DestroyChild();
					// Fill in Structure !!!
					DestroyScreenmap();
					Fire_OnScreenmapFSMCancel();					
					m_currentState = STATE_SCREENMAP_NODE_IDLE;
				} break;
				default: {
					ASSERT(0);
				} break;
			}
		} break; //case STATE_SCREENMAP_NODE_BUSY:

	} // switch (CurrentState)
}

BOOL CNodeLCDScreenmapFSMMaster::Update() {
	if (m_currentState == STATE_SCREENMAP_NODE_IDLE) {	
		if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {
			CNodeLCDScreenmapFSMInfo::GetInstance()->SetLock(TRUE);
		}
		FSM(FSM_EVENT_OPERATION_NODE_START);
		return TRUE;
	}
	return FALSE;
}

BOOL CNodeLCDScreenmapFSMMaster::CancelUpdateLCD(void) {
	if (m_pChildFSM != NULL) {
		m_pChildFSM->CancelUpdateLCD();
		return TRUE;
	}	
	return FALSE;
}
//////////////////////////////////////////////////////////////
BOOL CNodeLCDScreenmapFSMMaster::CheckNodeAddress(BYTE bNodeAddress) {
	if (m_pCurrNodeLCDScreenmaps != NULL) {
		if (m_pCurrNodeLCDScreenmaps->GetAssignedNode() != NULL) {
			return (bNodeAddress == m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress);				   
		}
	}
	return FALSE;
}

BYTE CNodeLCDScreenmapFSMMaster::GetNodeAddress() {
	if (m_pCurrNodeLCDScreenmaps != NULL) {
		if (m_pCurrNodeLCDScreenmaps->GetAssignedNode() != NULL) {
			return(m_pCurrNodeLCDScreenmaps->GetAssignedNode()->bLogicalAddress);
		}
	}
	return 0xFF;
}
////////////////////////////////////////////////////////////////

void CNodeLCDScreenmapFSMMaster::invoke_signalEvent(CNodeLCDScreenmapFSM* pChildFSM, BYTE bNodeAddress, void (CNodeLCDScreenmapFSM::*pf)()) {
	ASSERT(0!=pf);
	ASSERT(0!=pChildFSM);
	if ((CheckNodeAddress(bNodeAddress)==TRUE)&&(0!=pChildFSM)) {
		(pChildFSM->*pf)();
	}
}

void CNodeLCDScreenmapFSMMaster::EvSignalStatusInitDone(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusInitDone);
}
void CNodeLCDScreenmapFSMMaster::EvSignalStatusInitError(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusInitError);
}
void CNodeLCDScreenmapFSMMaster::EvSignalStatusEntryDone(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusEntryDone);
}
void CNodeLCDScreenmapFSMMaster::EvSignalStatusEntryError(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusEntryError);
}
void CNodeLCDScreenmapFSMMaster::EvSignalStatusNodeNotFound(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusNodeNotFound);
}
void CNodeLCDScreenmapFSMMaster::EvSignalStatusUnitNotFound(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusUnitNotFound);
}
void CNodeLCDScreenmapFSMMaster::EvAfterTimeout(void) {
	if (m_pChildFSM != NULL) {
		m_pChildFSM->EvAfterTimeout();
	}
}

// Voor de nieuwe touchscreen.
void CNodeLCDScreenmapFSMMaster::EvSignalStatusDoneOk(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusDoneOk);
}
void CNodeLCDScreenmapFSMMaster::EvSignalStatusDoneError(BYTE bNodeAddress) {
	invoke_signalEvent(m_pChildFSM,bNodeAddress,&CNodeLCDScreenmapFSM::EvSignalStatusDoneError);
}

//////////////////////////////////////////////////////////////////////////////////////

static void formatNodeInfo(CCanNode* pNode, BYTE bNodeAddress, CString* pStrNodeInfo) 
{
	CString szNodeName;
	if (NULL != pNode) {
		const char* str = pNode->getNodeName();
		if (NULL != str) {
			szNodeName = CString(str);
		}
	}

	if (!szNodeName.IsEmpty()) {
		pStrNodeInfo->Format("'%s', LA=0x%02x",szNodeName,pNode->bLogicalAddress);
	}
	else {
		pStrNodeInfo->Format("LA=0x%02x",bNodeAddress);
	}
}

void CNodeLCDScreenmapFSMMaster::Notify_OnStartScreenmap(BYTE bScreenmapID)
{
	if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {				
		const int index = m_iCurrentNode-1;
		CCanNode* const pNode = CNodeLCDScreenmapFSMInfo::GetInstance()->GetSmapCurrNode(index);

		CString strSmapIdentifier;
		const CScreenmapID screenmapID(bScreenmapID);
		CScreenmapUtil::format(screenmapID,&strSmapIdentifier);

		CString strNodeIdentifier;
		formatNodeInfo(pNode,this->GetNodeAddress(),&strNodeIdentifier);

		CString s;	
		s.Format("Updating node %s screenmap=%s",strNodeIdentifier, strSmapIdentifier);

		if (m_pComApplication != NULL) {
			m_pComApplication->NotifyEventScreenmapFSMInfo(s);
		}
	}
}

void CNodeLCDScreenmapFSMMaster::Notify_OnWriteScreenmapEntry(BYTE ,int ,int ) { }

void CNodeLCDScreenmapFSMMaster::Notify_OnInitScreenmapDone(BYTE ) { }

void CNodeLCDScreenmapFSMMaster::Notify_OnWriteScreenmapEntryDone(BYTE ,int ,int ) { }

void CNodeLCDScreenmapFSMMaster::Notify_OnInitScreenmapError(BYTE bScreenmapID)
{
	if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {	
		const int index = m_iCurrentNode-1;
		CCanNode* const pNode = CNodeLCDScreenmapFSMInfo::GetInstance()->GetSmapCurrNode(index);

		CString strSmapIdentifier;
		const CScreenmapID screenmapID(bScreenmapID);
		CScreenmapUtil::format(screenmapID,&strSmapIdentifier);

		CString strNodeIdentifier;
		formatNodeInfo(pNode,this->GetNodeAddress(),&strNodeIdentifier);

		CString s;	
		s.Format("Updating node %s screenmap=%s. Init screenmap error!",strNodeIdentifier, strSmapIdentifier);
	
		CNodeLCDScreenmapFSMInfo::GetInstance()->SetErrorMessage(s);

		if (m_pComApplication != NULL) {
			m_pComApplication->NotifyEventScreenmapFSMInfo(s);
		}	
	}
}

void CNodeLCDScreenmapFSMMaster::Notify_OnWriteScreenmapEntryError(BYTE bScreenmapID,int iEntryNR,int iMaxEntries)
{
	if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {				
		const int index = m_iCurrentNode-1;
		CCanNode* const pNode = CNodeLCDScreenmapFSMInfo::GetInstance()->GetSmapCurrNode(index);

		CString strSmapIdentifier;
		const CScreenmapID screenmapID(bScreenmapID);
		CScreenmapUtil::format(screenmapID,&strSmapIdentifier);

		CString strNodeIdentifier;
		formatNodeInfo(pNode,this->GetNodeAddress(),&strNodeIdentifier);

		CString s;	
		s.Format("Updating node %s screenmap=%s. Write entry %d/%d error!",strNodeIdentifier, strSmapIdentifier, iEntryNR, iMaxEntries-1);

		CNodeLCDScreenmapFSMInfo::GetInstance()->SetErrorMessage(s);

		if (m_pComApplication != NULL) { 
			m_pComApplication->NotifyEventScreenmapFSMInfo(s);
		}
	}
}

void CNodeLCDScreenmapFSMMaster::Notify_OnScreenmapErrorNode(BYTE bScreenmapID,BYTE bNodeError)
{
	if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {
		
		const int index = m_iCurrentNode-1;
		CCanNode* const pNode =  CNodeLCDScreenmapFSMInfo::GetInstance()->GetSmapCurrNode(index);

		CString strSmapIdentifier;
		const CScreenmapID screenmapID(bScreenmapID);
		CScreenmapUtil::format(screenmapID,&strSmapIdentifier);

		CString strNodeIdentifier;
		formatNodeInfo(pNode,this->GetNodeAddress(),&strNodeIdentifier);	

		CString s;	
		s.Format("Updating node %s screenmap=%s. Node 0x%02x not found in the database. BROWSE FIRST via the master.",
				 strNodeIdentifier, strSmapIdentifier, bNodeError);
		
	
		CNodeLCDScreenmapFSMInfo::GetInstance()->SetErrorMessage(s);
	
		if (m_pComApplication != NULL) {
			m_pComApplication->NotifyEventScreenmapFSMInfo(s);
		}
	}
}

void CNodeLCDScreenmapFSMMaster::Notify_OnScreenmapErrorUnit(BYTE bScreenmapID,BYTE bNodeError,BYTE bUnitError)
{
	if (CNodeLCDScreenmapFSMInfo::GetInstance() != NULL) {
				
		const int index = m_iCurrentNode-1;
		CCanNode* const pNode =  CNodeLCDScreenmapFSMInfo::GetInstance()->GetSmapCurrNode(index);
		
		CString strSmapIdentifier;
		const CScreenmapID screenmapID(bScreenmapID);
		CScreenmapUtil::format(screenmapID,&strSmapIdentifier);

		CString strNodeIdentifier;
		formatNodeInfo(pNode,this->GetNodeAddress(),&strNodeIdentifier);	

		CString s;	
		s.Format("Updating node %s screenmap=%s. - Unit 0x%02x,0x%02x not found in the database. BROWSE FIRST via the master.",
				 strNodeIdentifier, strSmapIdentifier, bNodeError, bUnitError);
				
		CNodeLCDScreenmapFSMInfo::GetInstance()->SetErrorMessage(s);
		
		if (m_pComApplication != NULL) {
			m_pComApplication->NotifyEventScreenmapFSMInfo(s);
		}
	}
}

void CNodeLCDScreenmapFSMMaster::Start_TimeoutTimer(void)  {
	if (m_pComApplication != NULL) {		
		m_pComApplication->StartTimer(TIMERID_SCREENMAP_ACKTIMEOUT, TIMER_SCREENMAP_ACKTIMEOUT_VALUE);	
	}
}

void CNodeLCDScreenmapFSMMaster::Stop_TimeoutTimer(void) {
	if (m_pComApplication != NULL) {
		m_pComApplication->StopTimer( TIMERID_SCREENMAP_ACKTIMEOUT );		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
void CNodeLCDScreenmapFSMMaster::Fire_OnScreenmapFSMDone() {
	if (m_pComApplication != NULL) {
		CNodeLCDScreenmapFSMInfo::GetInstance()->Set_SuccessFlag();	
		m_pComApplication->NotifyOnTaskDone(COMAPPLICATION_TASK_ID_SCREENMAP_UPDATE_ENGINE);
	}
}

void CNodeLCDScreenmapFSMMaster::Fire_OnScreenmapFSMError() {
	if (m_pComApplication != NULL)  {
		CNodeLCDScreenmapFSMInfo::GetInstance()->Set_ErrorFlag();	
		m_pComApplication->NotifyOnTaskError(COMAPPLICATION_TASK_ID_SCREENMAP_UPDATE_ENGINE);
	}
}

void CNodeLCDScreenmapFSMMaster::Fire_OnScreenmapFSMCancel() {
	if (m_pComApplication != NULL)  {
		CNodeLCDScreenmapFSMInfo::GetInstance()->Set_CancelFlag();	
		m_pComApplication->NotifyOnTaskCancelled(COMAPPLICATION_TASK_ID_SCREENMAP_UPDATE_ENGINE);
	}
}

/**
 * Send When screenmap for a node has been succesfully send ...
 */
void CNodeLCDScreenmapFSMMaster::Event_ScreenmapDoneOk() {
	Trigger(FSM_EVENT_SIGNAL_NODE_DONE);
}

/**
 * Send When screenmap for a node has not!!! been succesfully send ...
 */
void CNodeLCDScreenmapFSMMaster::Event_ScreenmapDoneError() {
	Trigger(FSM_EVENT_SIGNAL_NODE_ERROR);
}

/**
 * Send When screenmap for a node has not!!! been succesfully send ,
 * cancelled by user !!
 */
void CNodeLCDScreenmapFSMMaster::Event_ScreenmapDoneCancel() {
	Trigger(FSM_EVENT_SIGNAL_NODE_CANCEL);
}

