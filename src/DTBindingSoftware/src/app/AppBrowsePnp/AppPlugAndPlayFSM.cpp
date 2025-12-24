// PnpEngine.cpp: implementation of the CPnpEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppPlugAndPlayFSM.h"			
#include "NodeDatabase.h"
#include "TxNodeMessage.h"
#include "DebugMacros.h"
#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
namespace {
	void clear(CList<int>* pList) {
		pList->RemoveAll();
	}

	bool find(CList<int>* pList,int value) {
		const POSITION  pos = pList->Find(value);
		return (NULL == pos ? false : true);
	}

	bool add(CList<int>* pList,int value) {
		if (NULL != pList->AddTail(value)) {
			return true;
		}
		return false;
	}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const int CPnpEngine::BROWSE_NODE_TYPE_FLAGS_ALL = ( NODE_PIN_STD_UNITS | NODE_LCD | NODE_RESERVED | NODE_PIN_GATEWAY);
const int CPnpEngine::BROWSE_NODE_TYPE_FLAGS_EXTRA = ( NODE_LCD | NODE_PIN_GATEWAY);
//////////////////////////////////////////////////////////////////////
CPnpEngine::CPnpEngine(BYTE bMode,IBrowsePnPEngine *pEngine,CNodeDatabase *pNodes) :
m_pf_CurrentMode( 0 ),
m_pDatabase( pNodes ),
m_pBrowsePnpEngine( pEngine ),
m_pTxPnpMessages( new CTXBrowsePnpMessages ),
m_listPhysicalAddress( ), m_queuePhysicalAddressPendingAck( ),
m_CurrentBrowseNodesCounter( 0 ),
m_PreviousBrowseNodes( 0 ),
m_bLogicalAddress(FIRST_VALID_LOGICAL_ADDRESS),
m_CurrentPnpState( STATE_PNP_IDLE ),
m_RetryCount( 0 ),
m_maxRetryCountValue( 5 ), 
m_NrOfNodesToAssignAddresses(0),
m_searchNormalNodes(false)
{
	if (bMode == PNP_MODE_AUTOMATIC) {
		this->m_pf_CurrentMode = &CPnpEngine::FSMAutoPnp;
	}
	else {
		this->m_pf_CurrentMode = &CPnpEngine::FSMRetrieveInfo;
	}
}

CPnpEngine::~CPnpEngine() {
	if (m_pTxPnpMessages != NULL) {
		delete m_pTxPnpMessages;
		m_pTxPnpMessages = NULL;
	}
}

void CPnpEngine::Start() {
	ASSERT( m_pf_CurrentMode != 0);
	PnpStateMachine(EV_PNP_BROWSE_PHYSICAL,0,0);
}

void CPnpEngine::Stop() {
	ASSERT( m_pf_CurrentMode != 0);
	PnpStateMachine(EV_PNP_USER_CANCEL,0,0);
}

void CPnpEngine::PnpStateMachine(int pnpEvent,int wParam,int lParam) {
	ASSERT( m_pf_CurrentMode );
	if (m_pf_CurrentMode != NULL) {
		(this->*m_pf_CurrentMode)(pnpEvent,wParam,lParam);
	}
}

// Sedert V16.43.
BOOL CPnpEngine::sendPnpPhysicalAck(unsigned long ulPhysicalAddress) {
	if (NULL == this->m_pBrowsePnpEngine) {
		INVARIANT(NULL != this->m_pBrowsePnpEngine);
		return FALSE;
	}
	return this->m_pBrowsePnpEngine->sendPnpPhysicalAck(ulPhysicalAddress);		
}

/*****************************************************************************************/
// Testcase: 
// Lijst van addressen die een physical ACk moeten krijgen opvullen met dummy
// waarden. Controleren of de DT18-PRO / USB-Modem deze kan verwerken.
//
// Via de DT18-PRO Testcase ok.
#undef DEBUG_TESTCASE_PHYSICAL_ADDRESS_ACK
#if (_DEBUG)
#define DEBUG_TESTCASE_PHYSICAL_ADDRESS_ACK				0
#else
#define DEBUG_TESTCASE_PHYSICAL_ADDRESS_ACK				0
#endif 

#if(DEBUG_TESTCASE_PHYSICAL_ADDRESS_ACK)
void dbg_test_AddPhysicalAddresses(CPnpEngine::CQueue* pQueue) {
	if (pQueue->GetCount() > 1) {
		return;
	}
	const int count = 1024;	// 254;	// Aantal addressen die ingevoegd worden.
	int physicalAddress = 0x0001;
	for (int i=0;i<count;++i) {
		const int entry = physicalAddress + i;
		if (!find(pQueue,entry)) {
			pQueue->Add(physicalAddress + i);
		}
	}
}

#define EXECUTE_TESTCASE_PHYSICAL_ACK(x)		dbg_test_AddPhysicalAddresses(x)
#else 
#define EXECUTE_TESTCASE_PHYSICAL_ACK(x)
#endif 
/*****************************************************************************************/

// Sedert V16.44
CPnpEngine::ResultBrowsePhysicalNodes_t CPnpEngine::checkDatabaseFoundDevices(BOOL checkForFactoryAddresses) {
	int nrOfNodes = 0;						
	if (!this->m_pDatabase->NROfNodes(&nrOfNodes)) {			
		m_pBrowsePnpEngine->Fire_OnPNPError();
		return RESULT_BROWSE_PHYSICALNODES_NO_NODES_FOUND;
	}
	if (0 == nrOfNodes) {
		return RESULT_BROWSE_PHYSICALNODES_NO_NODES_FOUND;
	}
						
	for (int i=0;i<nrOfNodes;i++)  {
		CCanNode* pNodeCheck = NULL;
		m_pDatabase->GetNode(i,&pNodeCheck);
		for(int t=0;t<nrOfNodes;t++) {
			CCanNode* pNode = NULL;
			m_pDatabase->GetNode(t,&pNode);
			if ((NULL != pNode) && (NULL != pNodeCheck)) {
				// Controle duplicates.
				if ((pNode->bLogicalAddress == pNodeCheck->bLogicalAddress) && (pNode->bLogicalAddress != NODE_FACTORY_ADDRESS) && (pNode != pNodeCheck)) {
					CString szMessage;
					szMessage.Format(_T("Duplicate Addresses : PH Address = 0x%06x - Logical address 0x%02x"),pNode->ulPhysicalAddress,pNode->bLogicalAddress);
					CDisplayDevice::DisplayErrorMsg(szMessage);										
					return RESULT_BROWSE_PHYSICALNODES_DUPLICATES_FOUND;								
				}
				// Controle factory addresses.
				else if (TRUE == checkForFactoryAddresses) {
					if (pNode->bLogicalAddress == NODE_FACTORY_ADDRESS) {
						CString szMessage;
						szMessage.Format(_T("Nodes with 0xFE Found!"),pNode->bLogicalAddress);
						CDisplayDevice::DisplayErrorMsg(szMessage);									
						return RESULT_BROWSE_PHYSICALNODES_FACTORY_ADDRESSES_FOUND;	
					}		
				}
			}
		}						
	}		
	return RESULT_BROWSE_PHYSICALNODES_SUCCESS;
}


// Sedert V16.31:
// Physical Node address wordt meegegeven bij het event EV_PNP_PHYSICAL_ADDRESS
void CPnpEngine::FSMRetrieveInfo(int pnpEvent,int wParam,int lParam) {
	switch(m_CurrentPnpState) {
		case STATE_PNP_IDLE: {
			PNP_DEBUG_TRACE("\nSTATE_PNP_IDLE - ");
			switch (pnpEvent) {
				case EV_PNP_BROWSE_PHYSICAL: {
					PNP_DEBUG_TRACE("EV_PNP_BROWSE_PHYSICAL");					
					CString szMessage(_T("Browse for physical nodes..."));
					CDisplayDevice::DisplayInfoMsg(szMessage);
					this->m_bLogicalAddress = FIRST_VALID_LOGICAL_ADDRESS;
					this->m_CurrentBrowseNodesCounter = 0;
					this->m_PreviousBrowseNodes = 0;
					this->m_NodeCount = 0;		
					this->m_RetryCount = 0;	
					this->m_searchNormalNodes = false;
					clear(&this->m_listPhysicalAddress);				// V16.31: Lijst van physical addresses			
					clear(&this->m_queuePhysicalAddressPendingAck);		// V16.43: Lijst van physical addressen waar er een ACK moet verstuurd worden.
					this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_ALL);
					this->m_maxRetryCountValue = this->getMaxPnpRetries(this->m_CurrentBrowseNodesCounter);
					this->m_pBrowsePnpEngine->StartPnPTimer(this->getTimerPnpReloadValue(this->m_CurrentBrowseNodesCounter));
					this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
					this->m_CurrentPnpState = STATE_PNP_BROWSE_PHYSICAL;					
				} break; // EV_PNP_BROWSE_PHYSICAL

				case EV_PNP_TIMEOUT:
				case EV_PNP_ADDRESS_ASSIGNED:
				case EV_PNP_PHYSICAL_ADDRESS: {
					PNP_DEBUG_TRACE("DEFAULT EVENT");		
				} break; 

				case EV_PNP_USER_CANCEL: {
					PNP_DEBUG_TRACE("EV USER CANCEL");
					m_CurrentPnpState = STATE_PNP_IDLE;
					m_pBrowsePnpEngine->Fire_OnPNPCancel();
				}break;
			}//end switch event ...
		}break;	

		case STATE_PNP_BROWSE_PHYSICAL: {
			PNP_DEBUG_TRACE("\nSTATE_PNP_BROWSE_PHYSICAL - ");
			switch(pnpEvent) {				
				case EV_PNP_PHYSICAL_ADDRESS: {
					// Sedert V16.31:
					// Wanneer het physical address nog niet aanwezig is in de lijst.
					PNP_DEBUG_TRACE("EV_PNP_PHYSICAL_ADDRESS");
				
					// Lijst van de gevonden physical addresses.
					if (!find(&this->m_listPhysicalAddress,wParam)) {
						add(&this->m_listPhysicalAddress,wParam);
						this->m_NodeCount++;
					}

					// Testcase zie macro DEBUG_TESTCASE_PHYSICAL_ADDRESS_ACK
					// We gaan de lijst opvullen met dummy waarden.
					// Dit zorgt ervoor dat er een meerdere FC_PNPPHYSADRACK verstuurd worden.
					// Via deze kunnen we controleren of er geen probeem is met de buffers.
					EXECUTE_TESTCASE_PHYSICAL_ACK(&this->m_queuePhysicalAddressPendingAck);

					// Lijst van de devices waar er een Physical naar verstuurd moet worden.
					if (!find(&this->m_queuePhysicalAddressPendingAck,wParam)) {
						this->m_queuePhysicalAddressPendingAck.Add(wParam);
					}
				} break; // EV_PNP_PHYSICAL_ADDRESS:

				case EV_PNP_POLL_FSM: {							
					const int physicalAddress = this->m_queuePhysicalAddressPendingAck.getFirst();
					this->m_pBrowsePnpEngine->StopPnpPollTimer();
					if (physicalAddress >= 0) {
						this->sendPnpPhysicalAck(physicalAddress);
					}		
					this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_RELOAD_VALUE_MSEC);
				} break;

				case EV_PNP_TIMEOUT: {
					PNP_DEBUG_TRACE("EV_PNP_TIMEOUT");					
					this->m_CurrentBrowseNodesCounter = this->m_NodeCount;
					this->m_NodeCount=0;
					if (0 == this->m_CurrentBrowseNodesCounter) {
						CString szMessage(_T("No nodes found or link not open !"));
						CDisplayDevice::DisplayErrorMsg(szMessage);						
						this->m_CurrentPnpState = STATE_PNP_IDLE;
						this->m_pBrowsePnpEngine->Fire_OnPNPError();
					}	
					else if ((this->m_CurrentBrowseNodesCounter == this->m_PreviousBrowseNodes) || (this->m_RetryCount >= this->m_maxRetryCountValue)) {				
						static const BOOL CHECK_DATABASE_FOR_FACTORY_ADDRESSES_YES = TRUE;
						const ResultBrowsePhysicalNodes_t res = this->checkDatabaseFoundDevices(CHECK_DATABASE_FOR_FACTORY_ADDRESSES_YES);
						switch(res) {
							case RESULT_BROWSE_PHYSICALNODES_FACTORY_ADDRESSES_FOUND: {
								this->m_pBrowsePnpEngine->Fire_OnFactoryAddressFound();						  
							} break;
							case RESULT_BROWSE_PHYSICALNODES_DUPLICATES_FOUND: {
								this-> m_pBrowsePnpEngine->Fire_OnDuplicatesFound();
							} break;
							case RESULT_BROWSE_PHYSICALNODES_NO_NODES_FOUND: {
								CString szMessage(_T("No nodes found or link not open !"));
								CDisplayDevice::DisplayErrorMsg(szMessage);						
								this->m_CurrentPnpState = STATE_PNP_IDLE;
								this->m_pBrowsePnpEngine->Fire_OnPNPError();										
							} break;
							case RESULT_BROWSE_PHYSICALNODES_SUCCESS: {		
								// Sedert V16.45: Workaround nodes 0xFFFFFF
								// Vanaf dit aantal nodes nog eens een search doen specifiek op de standaard nodes.
								if (this->m_CurrentBrowseNodesCounter >= THRESSHOLD_SEARCH_EXTRA_NODES_NORMAL) {
									int nrOfNodes = 0;
									if (NULL != this->m_pDatabase) {
										this->m_pDatabase->NROfNodes(&nrOfNodes);
									}
									CString szMessage;
									szMessage.Format("Browse for physical nodes again... Found %d physical nodes. Search extra nodes.",nrOfNodes);
									CDisplayDevice::DisplayInfoMsg(szMessage);						
																		
									this->m_pBrowsePnpEngine->StopPnpPollTimer();															
									this->m_pTxPnpMessages->StartPhysicalBrowse(NODE_PIN_STD_UNITS);									
									this->m_pBrowsePnpEngine->StartPnPTimer(this->getTimerPnpReloadValue(this->m_CurrentBrowseNodesCounter));
									this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
									this->m_maxRetryCountValue = PNP_SEARCH_EXTRA_NODES_MAX_RETRIES;
									this->m_PreviousBrowseNodes = nrOfNodes;	// Initialiseren op een verschillende waarde dan m_CurrentBrowseNodesCounter
									this->m_CurrentBrowseNodesCounter = 0;
									this->m_NodeCount=0;	
									this->m_RetryCount = 0;		
									this->m_searchNormalNodes = true;
									this->m_CurrentPnpState = STATE_PNP_BROWSE_PHYSICAL_EXTRA_NODES;							
								}
								// Sedert V16.41: Workaround nodes 0xFFFFFF
								// Vanaf dit aantal nodes nog eens een search doen specifiek op de LCD en gateway nodes.
								else if (this->m_CurrentBrowseNodesCounter >= THRESSHOLD_SEARCH_EXTRA_NODES) {									
									int nrOfNodes = 0;
									if (NULL != this->m_pDatabase) {
										this->m_pDatabase->NROfNodes(&nrOfNodes);
									}
									CString szMessage;
									szMessage.Format("Browse for physical nodes again... Found %d physical nodes. Search extra nodes.",nrOfNodes);
									CDisplayDevice::DisplayInfoMsg(szMessage);
						
									this->m_PreviousBrowseNodes = nrOfNodes;	// Initialiseren op een verschillende waarde dan m_CurrentBrowseNodesCounter
									this->m_CurrentBrowseNodesCounter = 0;
									this->m_NodeCount=0;	
									clear(&this->m_listPhysicalAddress);		// Sedert V16.31: Lijst bijhouden van de physical addressen.	
									this->m_pBrowsePnpEngine->StopPnpPollTimer();						
									this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_EXTRA);		
									this->m_pBrowsePnpEngine->StartPnPTimer(TIMER_PNP_BROADCAST_RELOAD_EXTRA_NODES_VALUE_MSEC);
									this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
									this->m_maxRetryCountValue = PNP_SEARCH_EXTRA_NODES_MAX_RETRIES;
									this->m_RetryCount = 0;		
									this->m_searchNormalNodes = false;
									this->m_CurrentPnpState = STATE_PNP_BROWSE_PHYSICAL_EXTRA_NODES;
								}
								else {
									int nrOfNodes = 0;
									if (NULL != this->m_pDatabase) {
										this->m_pDatabase->NROfNodes(&nrOfNodes);
									}
									CString szMessage;
									szMessage.Format("Browse for physical nodes done... Found %d physical nodes",nrOfNodes);
									CDisplayDevice::DisplayInfoMsg(szMessage);

									this->m_pBrowsePnpEngine->Fire_OnPNPOK();
								}
							} break;
							default: {
								ASSERT(0); 
							} break;
						}
					}
					else {				
						// Sedert V16.31.
						// Gaan minimum 2x een physical browse for nodes laten uitvoeren.
						CString szMessage;
						if (this->m_RetryCount > 0) {
							szMessage.Format("Browse for physical nodes again... Found %d physical nodes.", this->m_CurrentBrowseNodesCounter);												
							this->m_PreviousBrowseNodes = this->m_CurrentBrowseNodesCounter;	// !!!
						}
						else {
							this->m_PreviousBrowseNodes = 0;
							szMessage.Format("Browse for physical nodes again...");													
						}																	
						CDisplayDevice::DisplayInfoMsg(szMessage);

						this->m_NodeCount=0;	
						clear(&this->m_listPhysicalAddress);		// Sedert V16.31: Lijst bijhouden van de physical addressen.	
						this->m_pBrowsePnpEngine->StopPnpPollTimer();						
						this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_ALL);		
						this->m_pBrowsePnpEngine->StartPnPTimer(this->getTimerPnpReloadValue(this->m_CurrentBrowseNodesCounter));
						this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
						this->m_maxRetryCountValue = this->getMaxPnpRetries(m_CurrentBrowseNodesCounter);
						this->m_RetryCount++;		
				   }
				} break; // EV_PNP_TIMEOUT
				
				case EV_PNP_USER_CANCEL: {
					PNP_DEBUG_TRACE("EV USER CANCEL");
					this->m_CurrentPnpState = STATE_PNP_IDLE;
					this->m_pBrowsePnpEngine->Fire_OnPNPCancel();
				}break;				
			}
		}break;

		case STATE_PNP_BROWSE_PHYSICAL_EXTRA_NODES: {											
			switch(pnpEvent) {				
				case EV_PNP_PHYSICAL_ADDRESS: {
					PNP_DEBUG_TRACE("EV_PNP_PHYSICAL_ADDRESS");			
					// Lijst van de gevonden physical addresses.
					if (!find(&this->m_listPhysicalAddress,wParam)) {
						add(&this->m_listPhysicalAddress,wParam);
						this->m_NodeCount++;
					}
					// Lijst van de devices waar er een Physical naar verstuurd moet worden.
					if (!find(&this->m_queuePhysicalAddressPendingAck,wParam)) {
						this->m_queuePhysicalAddressPendingAck.Add(wParam);
					}
				} break; // EV_PNP_PHYSICAL_ADDRESS:

				case EV_PNP_TIMEOUT: {
					PNP_DEBUG_TRACE("EV_PNP_TIMEOUT");					
					this->m_CurrentBrowseNodesCounter = this->m_NodeCount;
					this->m_NodeCount=0;

					// Sedert V16.45:
					// Indien we een extra search doen voor de std nodes, ook nog eens zoeken naar LCD en gateway nodes.
					if (this->m_searchNormalNodes) {					
						int nrOfNodes = 0;
						if (NULL != this->m_pDatabase) {
							this->m_pDatabase->NROfNodes(&nrOfNodes);
						}
						
						CString szMessage;
						szMessage.Format("Browse for physical nodes again... Found %d physical nodes. Search extra nodes.",nrOfNodes);
						CDisplayDevice::DisplayInfoMsg(szMessage);
						
						this->m_PreviousBrowseNodes = nrOfNodes;	// Initialiseren op een verschillende waarde dan m_CurrentBrowseNodesCounter
						this->m_CurrentBrowseNodesCounter = 0;
						this->m_NodeCount=0;	
						clear(&this->m_listPhysicalAddress);		// Sedert V16.31: Lijst bijhouden van de physical addressen.	
						this->m_pBrowsePnpEngine->StopPnpPollTimer();						
						this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_EXTRA);		
						this->m_pBrowsePnpEngine->StartPnPTimer(TIMER_PNP_BROADCAST_RELOAD_EXTRA_NODES_VALUE_MSEC);
						this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
						this->m_maxRetryCountValue = PNP_SEARCH_EXTRA_NODES_MAX_RETRIES;
						this->m_RetryCount = 0;		
						this->m_CurrentPnpState = STATE_PNP_BROWSE_PHYSICAL_EXTRA_NODES;					
						this->m_searchNormalNodes = false;
					}
					else if ((this->m_CurrentBrowseNodesCounter == this->m_PreviousBrowseNodes) || (this->m_RetryCount >= this->m_maxRetryCountValue)) {	
						static const BOOL CHECK_DATABASE_FOR_FACTORY_ADDRESSES_YES = TRUE;
						const ResultBrowsePhysicalNodes_t res = this->checkDatabaseFoundDevices(CHECK_DATABASE_FOR_FACTORY_ADDRESSES_YES);
						switch(res) {
							case RESULT_BROWSE_PHYSICALNODES_FACTORY_ADDRESSES_FOUND: {
								this->m_pBrowsePnpEngine->Fire_OnFactoryAddressFound();						  
							} break;
							case RESULT_BROWSE_PHYSICALNODES_DUPLICATES_FOUND: {
								this-> m_pBrowsePnpEngine->Fire_OnDuplicatesFound();
							} break;
							case RESULT_BROWSE_PHYSICALNODES_NO_NODES_FOUND: {
								ASSERT(0);	// Kan niet voorkomen.
								CString szMessage(_T("No nodes found or link not open !"));
								CDisplayDevice::DisplayErrorMsg(szMessage);						
								this->m_CurrentPnpState = STATE_PNP_IDLE;
								this->m_pBrowsePnpEngine->Fire_OnPNPError();										
							} break;
							case RESULT_BROWSE_PHYSICALNODES_SUCCESS: {
								int nrOfNodes = 0;
								if (NULL != this->m_pDatabase) {
									this->m_pDatabase->NROfNodes(&nrOfNodes);
								}
								CString szMessage;
								szMessage.Format("Browse for physical nodes done... Found %d physical nodes",nrOfNodes);
								CDisplayDevice::DisplayInfoMsg(szMessage);

								this->m_pBrowsePnpEngine->Fire_OnPNPOK();						  
							} break;
							default: {
								ASSERT(0); 
							} break;
						}
					}
					else {						
						int nrOfNodes = 0;
						if (NULL != this->m_pDatabase) {
							this->m_pDatabase->NROfNodes(&nrOfNodes);
						}

						CString szMessage;
						szMessage.Format("Browse for physical nodes again... Found %d physical nodes. Search extra nodes.",nrOfNodes);
						CDisplayDevice::DisplayInfoMsg(szMessage);

						this->m_PreviousBrowseNodes = this->m_CurrentBrowseNodesCounter;
						this->m_NodeCount=0;	
						clear(&this->m_listPhysicalAddress);		// Sedert V16.31: Lijst bijhouden van de physical addressen.	
						this->m_pBrowsePnpEngine->StopPnpPollTimer();						
						this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_EXTRA);		
						this->m_pBrowsePnpEngine->StartPnPTimer(TIMER_PNP_BROADCAST_RELOAD_EXTRA_NODES_VALUE_MSEC);
						this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
						this->m_maxRetryCountValue = PNP_SEARCH_EXTRA_NODES_MAX_RETRIES;
						this->m_RetryCount++;							
					}
				} break;

				case EV_PNP_POLL_FSM: {							
					const int physicalAddress = m_queuePhysicalAddressPendingAck.getFirst();
					m_pBrowsePnpEngine->StopPnpPollTimer();
					if (physicalAddress >= 0) {
						this->sendPnpPhysicalAck(physicalAddress);
					}		
					m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_RELOAD_VALUE_MSEC);
				} break;

				case EV_PNP_USER_CANCEL: {
					PNP_DEBUG_TRACE("EV USER CANCEL");
					this->m_CurrentPnpState = STATE_PNP_IDLE;
					this->m_pBrowsePnpEngine->Fire_OnPNPCancel();
				}break;		
			} /* end switch */
		} break;		
	}// end switch (currentstate)
}


BOOL CPnpEngine::helper_checkNodeFactoryAddressAndAssignNextAddress(CCanNode* pNode) {
	REQUIRE(NULL != pNode);
	if (pNode->bLogicalAddress != NODE_FACTORY_ADDRESS) {
		return TRUE;
	}

	// Check all nodes for a free logical address
	while(1) {
		CCanNode* pDummyNode = NULL;							
		int iEntryNr = 0;
		if (!(m_pDatabase->FindNode(this->m_bLogicalAddress,&iEntryNr,&pDummyNode))) {													
			pNode->bLogicalAddress = this->m_bLogicalAddress;
			ASSERT((pNode->bLogicalAddress>=FIRST_VALID_LOGICAL_ADDRESS) && (pNode->bLogicalAddress<=LAST_VALID_LOGICAL_ADDRESS));
			return TRUE;
		}
		else {
			this->m_bLogicalAddress++;
			if (this->m_bLogicalAddress == LAST_VALID_LOGICAL_ADDRESS) {
    	    	CString szMessage(_T("Max NR of nodes reached. All logical addresses used!"));
				CDisplayDevice::DisplayErrorMsg(szMessage);						
				this->m_CurrentPnpState = STATE_PNP_IDLE;										
				this->m_pBrowsePnpEngine->Fire_OnPNPError();	// SEND EVENT - FAILED 
				return FALSE;
			}
		}
	}			
	ASSERT(0);
	return FALSE;
}

void CPnpEngine::FSMAutoPnp(int pnpEvent,int wParam,int lParam) {
	switch(m_CurrentPnpState) {
		case STATE_PNP_IDLE: {
			PNP_DEBUG_TRACE("\nSTATE_PNP_IDLE - ");
			switch (pnpEvent) {
				case EV_PNP_BROWSE_PHYSICAL: {
					PNP_DEBUG_TRACE("EV_PNP_BROWSE_PHYSICAL");					
					CString szMessage(_T("Browse for physical nodes..."));
					CDisplayDevice::DisplayInfoMsg(szMessage);
					this->m_bLogicalAddress = FIRST_VALID_LOGICAL_ADDRESS;
					this->m_CurrentBrowseNodesCounter = 0;
					this->m_PreviousBrowseNodes = 0;
					this->m_NodeCount = 0;
					this->m_RetryCount = 0;	
					this->m_NrOfNodesToAssignAddresses = 0;
					clear(&this->m_listPhysicalAddress);				// V16.31: Lijst bijhouden van de physical addressen.	
					clear(&this->m_queuePhysicalAddressPendingAck);		// V16.43: Lijst van physical addressen waar er een ACK moet verstuurd worden.					
					this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_ALL);			
					this->m_maxRetryCountValue = this->getMaxPnpRetries(this->m_CurrentBrowseNodesCounter);
					this->m_pBrowsePnpEngine->StartPnPTimer(this->getTimerPnpReloadValue(this->m_CurrentBrowseNodesCounter));			
					this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
					this->m_CurrentPnpState = STATE_PNP_BROWSE_PHYSICAL;									
				} break; // EV_PNP_BROWSE_PHYSICAL

				case EV_PNP_TIMEOUT:
				case EV_PNP_ADDRESS_ASSIGNED:
				case EV_PNP_PHYSICAL_ADDRESS: {
					PNP_DEBUG_TRACE("DEFAULT EVENT");		// absorb
				} break; 

				case EV_PNP_USER_CANCEL: {
					PNP_DEBUG_TRACE("EV USER CANCEL");
					this->m_CurrentPnpState = STATE_PNP_IDLE;
					this->m_pBrowsePnpEngine->Fire_OnPNPCancel();
				}break;
			}
		}break;

		case STATE_PNP_BROWSE_PHYSICAL: {
			PNP_DEBUG_TRACE("\nSTATE_PNP_BROWSE_PHYSICAL - ");
			switch(pnpEvent) {
				case EV_PNP_PHYSICAL_ADDRESS: {
				   PNP_DEBUG_TRACE("EV_PNP_PHYSICAL_ADDRESS");				  
				   	// Sedert V16.31:
					// Wanneer het physical address nog niet aanwezig is in de lijst.
				  	if (!find(&this->m_listPhysicalAddress,wParam)) {
						add(&this->m_listPhysicalAddress,wParam);
						this->m_NodeCount++;
					}
					// Lijst van de devices waar er een Physical naar verstuurd moet worden.
					if (!find(&this->m_queuePhysicalAddressPendingAck,wParam)) {
						this->m_queuePhysicalAddressPendingAck.Add(wParam);
					}
				} break; // EV_PNP_PHYSICAL_ADDRESS:

				case EV_PNP_POLL_FSM: {					
					const int physicalAddress = this->m_queuePhysicalAddressPendingAck.getFirst();
					this->m_pBrowsePnpEngine->StopPnpPollTimer();
					if (physicalAddress >= 0) {
						this->sendPnpPhysicalAck(physicalAddress);
					}		
					this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_RELOAD_VALUE_MSEC);
				} break;

				case EV_PNP_TIMEOUT: {
					PNP_DEBUG_TRACE("EV_PNP_TIMEOUT");					
					this->m_CurrentBrowseNodesCounter = this->m_NodeCount;
					this->m_NodeCount=0;
					if (0 == this->m_CurrentBrowseNodesCounter) {
						CString szMessage(_T("No nodes found or link not open !"));
						CDisplayDevice::DisplayErrorMsg(szMessage);						
						this->m_CurrentPnpState = STATE_PNP_IDLE;
						this->m_pBrowsePnpEngine->Fire_OnPNPError();										       	   			  
					}	
					else if ((this->m_CurrentBrowseNodesCounter == this->m_PreviousBrowseNodes) || (this->m_RetryCount >= this->m_maxRetryCountValue)) {											
						if (this->m_CurrentBrowseNodesCounter >= THRESSHOLD_SEARCH_EXTRA_NODES) {
							CString szMessage;						
							szMessage.Format("Browse for physical nodes again... Found %d physical nodes. Search extra nodes.",this->m_CurrentBrowseNodesCounter);												
							CDisplayDevice::DisplayInfoMsg(szMessage);		
							this->m_CurrentBrowseNodesCounter = 0;
							this->m_PreviousBrowseNodes = 0;
							this->m_NodeCount = 0;
							this->m_RetryCount = 0;	
							clear(&this->m_listPhysicalAddress);				// V16.31: Lijst bijhouden van de physical addressen.										
							this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_EXTRA);			
							this->m_maxRetryCountValue = PNP_SEARCH_EXTRA_NODES_MAX_RETRIES;
							this->m_pBrowsePnpEngine->StartPnPTimer(TIMER_PNP_BROADCAST_RELOAD_EXTRA_NODES_VALUE_MSEC);			
							this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
							this->m_CurrentPnpState = STATE_PNP_BROWSE_PHYSICAL_EXTRA_NODES;										
						}
						else {					
							CString szMessage;
							int nrOfNodes = 0;
							this->m_pDatabase->NROfNodes(&nrOfNodes);
							this->m_NrOfNodesToAssignAddresses = nrOfNodes;
							// ASSERT(this->m_NrOfNodesToAssignAddresses == this->m_CurrentBrowseNodesCounter);
							szMessage.Format("Assign %d addresses via plug and play",this->m_NrOfNodesToAssignAddresses);
							CDisplayDevice::DisplayInfoMsg(szMessage);

							CCanNode* pNode = NULL;				
							this->m_pDatabase->GetNode(this->m_NodeCount++,&pNode);				              
							ASSERT(NULL != pNode);
							if (this->helper_checkNodeFactoryAddressAndAssignNextAddress(pNode)) {
								ASSERT(NODE_FACTORY_ADDRESS != pNode->bLogicalAddress);
							}					  						  
						
							szMessage.Format(_T("Node count %d: Assign logical address %02x to physical node %06x."),this->m_NodeCount, pNode->bLogicalAddress,pNode->ulPhysicalAddress);
							CDisplayDevice::DisplayInfoMsg(szMessage);
							
							this->m_pTxPnpMessages->AssignPhysicalAddress(pNode->ulPhysicalAddress,pNode->bLogicalAddress);
							this->m_pBrowsePnpEngine->StartPnPAckTimer();
							this->m_CurrentPnpState = STATE_PNP_ASSIGN_LA;		
						}
				   }
				   else {		
						// Sedert V16.31.
						// Gaan minimum 2x een physical browse for nodes laten uitvoeren.
					   	CString szMessage;
						if (this->m_RetryCount > 0) {
							szMessage.Format("Browse for physical nodes again... Found %d physical nodes.",this->m_CurrentBrowseNodesCounter);	
							this->m_PreviousBrowseNodes = this->m_CurrentBrowseNodesCounter;	// !!!
						}
						else {
							szMessage.Format("Browse for physical nodes again...");	
						}											
						CDisplayDevice::DisplayInfoMsg(szMessage);						

						this->m_NodeCount=0;		
						clear(&this->m_listPhysicalAddress);		// Sedert V16.31: Lijst bijhouden van de physical addressen.	
						this->m_pBrowsePnpEngine->StopPnpPollTimer();					
						this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_ALL);													
						this->m_pBrowsePnpEngine->StartPnPTimer(this->getTimerPnpReloadValue(m_CurrentBrowseNodesCounter));	
						this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
						this->m_maxRetryCountValue = this->getMaxPnpRetries(m_CurrentBrowseNodesCounter);
						this->m_RetryCount++;		
				   }
				} break; // EV_PNP_TIMEOUT
			
				case EV_PNP_BROWSE_PHYSICAL:
				case EV_PNP_ADDRESS_ASSIGNED: {
					PNP_DEBUG_TRACE("DEFAULT EVENT");	// absorb
				} break; 
				case EV_PNP_USER_CANCEL: {
					PNP_DEBUG_TRACE("EV USER CANCEL");
					this->m_CurrentPnpState = STATE_PNP_IDLE;
					this->m_pBrowsePnpEngine->Fire_OnPNPCancel();
				}break;

			} // switch(iEvent)		
		}break;

		case STATE_PNP_BROWSE_PHYSICAL_EXTRA_NODES: {
			switch(pnpEvent) {
				case EV_PNP_PHYSICAL_ADDRESS: {
				   PNP_DEBUG_TRACE("EV_PNP_PHYSICAL_ADDRESS");				  
				   	// Sedert V16.31:
					// Wanneer het physical address nog niet aanwezig is in de lijst.
				  	if (!find(&this->m_listPhysicalAddress,wParam)) {
						add(&this->m_listPhysicalAddress,wParam);
						this->m_NodeCount++;
					}
					// Lijst van de devices waar er een Physical naar verstuurd moet worden.
					if (!find(&this->m_queuePhysicalAddressPendingAck,wParam)) {
						this->m_queuePhysicalAddressPendingAck.Add(wParam);
					}
				} break; // EV_PNP_PHYSICAL_ADDRESS:

				case EV_PNP_POLL_FSM: {					
					const int physicalAddress = this->m_queuePhysicalAddressPendingAck.getFirst();
					this->m_pBrowsePnpEngine->StopPnpPollTimer();
					if (physicalAddress >= 0) {
						this->sendPnpPhysicalAck(physicalAddress);
					}		
					this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_RELOAD_VALUE_MSEC);
				} break;

				case EV_PNP_TIMEOUT: {		
					PNP_DEBUG_TRACE("EV_PNP_TIMEOUT");					
					this->m_CurrentBrowseNodesCounter = this->m_NodeCount;
					this->m_NodeCount=0;
					if ((this->m_CurrentBrowseNodesCounter == this->m_PreviousBrowseNodes) || (this->m_RetryCount >= this->m_maxRetryCountValue)) {							
						CString szMessage;
						int nrOfNodes = 0;
						this->m_pDatabase->NROfNodes(&nrOfNodes);
						this->m_NrOfNodesToAssignAddresses = nrOfNodes;						
						szMessage.Format("Assign %d addresses via plug and play",this->m_NrOfNodesToAssignAddresses);
						CDisplayDevice::DisplayInfoMsg(szMessage);

						CCanNode* pNode = NULL;
						this->m_pDatabase->GetNode(this->m_NodeCount++,&pNode);				              
						ASSERT(NULL != pNode);
						if (this->helper_checkNodeFactoryAddressAndAssignNextAddress(pNode)) {
							ASSERT(NODE_FACTORY_ADDRESS != pNode->bLogicalAddress);
						}					  						  						
						szMessage.Format(_T("Node count %d: Assign logical address %02x to physical node %06x."),this->m_NodeCount, pNode->bLogicalAddress, pNode->ulPhysicalAddress);
						CDisplayDevice::DisplayInfoMsg(szMessage);

						this->m_pTxPnpMessages->AssignPhysicalAddress(pNode->ulPhysicalAddress,pNode->bLogicalAddress);
						this->m_pBrowsePnpEngine->StartPnPAckTimer();
						this->m_CurrentPnpState = STATE_PNP_ASSIGN_LA;		
					}
					else {						
						int nrOfNodes = 0;
						if (NULL != this->m_pDatabase) {
							this->m_pDatabase->NROfNodes(&nrOfNodes);
						}
						CString szMessage;
						szMessage.Format("Browse for physical nodes again... Found %d physical nodes. Search extra nodes.",nrOfNodes);
						CDisplayDevice::DisplayInfoMsg(szMessage);

						this->m_PreviousBrowseNodes = this->m_CurrentBrowseNodesCounter;
						this->m_NodeCount=0;	
						clear(&this->m_listPhysicalAddress);		// Sedert V16.31: Lijst bijhouden van de physical addressen.	
						this->m_pBrowsePnpEngine->StopPnpPollTimer();						
						this->m_pTxPnpMessages->StartPhysicalBrowse(BROWSE_NODE_TYPE_FLAGS_EXTRA);		
						this->m_pBrowsePnpEngine->StartPnPTimer(TIMER_PNP_BROADCAST_RELOAD_EXTRA_NODES_VALUE_MSEC);
						this->m_pBrowsePnpEngine->StartPnpPollTimer(TIMER_PNP_POLL_INITIAL_RELOAD_VALUE_MSEC);
						this->m_maxRetryCountValue = PNP_SEARCH_EXTRA_NODES_MAX_RETRIES;
						this->m_RetryCount++;			
					}
				} break;
					
				case EV_PNP_BROWSE_PHYSICAL: 
				case EV_PNP_ADDRESS_ASSIGNED:{
					PNP_DEBUG_TRACE("DEFAULT EVENT");	// absorb
				} break; 
				case EV_PNP_USER_CANCEL: {
					PNP_DEBUG_TRACE("EV USER CANCEL");
					m_CurrentPnpState = STATE_PNP_IDLE;
					m_pBrowsePnpEngine->Fire_OnPNPCancel();
				}break;
			}
		} break;

		case STATE_PNP_ASSIGN_LA: {
			PNP_DEBUG_TRACE("\nSTATE_PNP_ASSIGN_LA - ");
			switch(pnpEvent) {						
				case EV_PNP_ADDRESS_ASSIGNED: {									
					this->m_pBrowsePnpEngine->StopPnPAckTimer();								
					int iResult = 0;
					if (this->m_NodeCount == this->m_NrOfNodesToAssignAddresses) {  								
						int iCurrentNROfNodes = 0;
						// CHECK FOR DUPLICATES						
						if (!this->m_pDatabase->NROfNodes(&iCurrentNROfNodes)) {						
							iResult = 0;
							this->m_pBrowsePnpEngine->Fire_OnPNPError();
						}
						else {							
							for (int i=0;i<iCurrentNROfNodes;i++) {
								CCanNode* pNodeCheck = 0;
								m_pDatabase->GetNode(i,&pNodeCheck);
								ASSERT(NULL != pNodeCheck);

								for(int t=0;t<iCurrentNROfNodes;t++) {
									CCanNode* pNode = NULL;
									m_pDatabase->GetNode(t,&pNode);
									ASSERT(NULL != pNode);

									if (NULL != pNode && NULL != pNodeCheck) {
										if ((pNode->bLogicalAddress == pNodeCheck->bLogicalAddress) && (pNode->bLogicalAddress != NODE_FACTORY_ADDRESS) && (pNode != pNodeCheck)){
											CString szMessage;
											szMessage.Format(_T("Duplicate Addresses : PH Address = 0x%06x - Logical address 0x%02x"),pNode->ulPhysicalAddress,pNode->bLogicalAddress);
											CDisplayDevice::DisplayErrorMsg(szMessage);								
											iResult = 1;
											break;										
										}
									}
								}
							}						
						}	

						this->m_CurrentPnpState = STATE_PNP_IDLE;					
						if (iResult == 0) {
							this->m_pBrowsePnpEngine->Fire_OnPNPOK();
						}
						else if (iResult == 1) {
							this->m_pBrowsePnpEngine->Fire_OnDuplicatesFound();
						}
					}
					else {
						CCanNode* pNode = NULL;
						m_pDatabase->GetNode(this->m_NodeCount++,&pNode);		
						
						ASSERT(NULL != pNode);
						if (helper_checkNodeFactoryAddressAndAssignNextAddress(pNode)) {
							ASSERT(NODE_FACTORY_ADDRESS != pNode->bLogicalAddress);
						}

						CString szMessage;
						szMessage.Format(_T("Node count %d: Assign logical address %02x to physical node %06x."),this->m_NodeCount, pNode->bLogicalAddress, pNode->ulPhysicalAddress);
						CDisplayDevice::DisplayInfoMsg(szMessage);

						this->m_pTxPnpMessages->AssignPhysicalAddress(pNode->ulPhysicalAddress,pNode->bLogicalAddress);
						this->m_pBrowsePnpEngine->StartPnPAckTimer();											
					}// end else
				} break; // EV_PNP_ADDRESS_ASSIGNED
			
				case EV_PNP_BROWSE_PHYSICAL:
				case EV_PNP_TIMEOUT:
				case EV_PNP_PHYSICAL_ADDRESS: {
					PNP_DEBUG_TRACE("DEFAULT EVENT");	// absorb
				} break; // EV_PNP_PHYSICAL_ADDRESS:

				case EV_PNP_USER_CANCEL: {				
					m_pBrowsePnpEngine->StopPnPAckTimer();
					PNP_DEBUG_TRACE("EV USER CANCEL");
					m_CurrentPnpState = STATE_PNP_IDLE;
					m_pBrowsePnpEngine->Fire_OnPNPCancel();
				}break;

				case EV_NORESPONSE: {				
					m_pBrowsePnpEngine->StopPnPAckTimer();
					PNP_DEBUG_TRACE("EV NORESPONSE");				
					m_CurrentPnpState = STATE_PNP_IDLE;
					m_pBrowsePnpEngine->Fire_OnPNPNoResponse();					
				}break;
			} // switch(iEvent)		
		}
		break;
		default:
			ASSERT(0);
		break;
	}
}

// Sedert V16.30:
// PNP reload waarde ifv het aantal gevonden modules.
//
// Voor deze aanpassing werd er altijd 23 seconden gewacht.
//
// Sedert V16.32: Timers verhoogd.
// Wanneer een node niet gevonden wordt via de Physical browse for nodes (en later toegevoegd wordt) via
// de gewone browse for nodes dan heeft hij physical node address 0xfffff.
// Dit komt af een toe eens voor.
//
// Sedert V16.43: Timer waarden hersteld zoals in V16.30.
// Hogere timerwaarden zijn geen oplossing voor nodes met adres 0xFFFFFF
int CPnpEngine::getTimerPnpReloadValue(int foundNodes) {
	enum {
		TIMER_RELOAD_VALUE_DEFAULT 						= 23000,	// 30000,	// 30 sec - vroeger 23 sec	
		TIMER_RELOAD_VALUE_THRESSHOLD_50MODULES			= 32000,	// 45000,	// 45 sec - vroeger 30 sec
		TIMER_RELOAD_VALUE_THRESSHOLD_100MODULES		= 60000,	// 75000	// 75 sec - vroeger 60 sec
		TIMER_RELOAD_VALUE_THRESSHOLD_150MODULES	    = 90000,	// 90000	// 120 sec - vroeger 90 sec
	};

	int retValue = TIMER_RELOAD_VALUE_DEFAULT;				
	if (foundNodes >= 150) {
		retValue = TIMER_RELOAD_VALUE_THRESSHOLD_150MODULES;
	}	
	else if (foundNodes >= 100) {
		retValue = TIMER_RELOAD_VALUE_THRESSHOLD_100MODULES;
	}
	else if (foundNodes >= 50) {
		retValue = TIMER_RELOAD_VALUE_THRESSHOLD_50MODULES;
	}
	return retValue;
}

// Sedert V16.30:
// Aantal keren dat er opnieuw een PNP broadcast mag gestuurd worden naar alle nodes.
// Nu gebeurd dit ifv het aantal gevonden nodes.
//
// Voor deze aanpassing was dit altijd 5 keer.
int CPnpEngine::getMaxPnpRetries(int foundNodes) {
	enum {
		MAX_RETRIES_VALUE_DEFAULT 						= 5,	
		MAX_RETRIES_VALUE_THRESSHOLD_50MODULES			= 8,	
		MAX_RETRIES_VALUE_THRESSHOLD_100MODULES			= 10,		
		MAX_RETRIES_VALUE_THRESSHOLD_150MODULES			= 14,	
	};
	
	int retValue = MAX_RETRIES_VALUE_DEFAULT;			
	if (foundNodes >= 150) {
		retValue = MAX_RETRIES_VALUE_THRESSHOLD_150MODULES;
	}
	else if (foundNodes >= 100) {
		retValue = MAX_RETRIES_VALUE_THRESSHOLD_100MODULES;
	}
	else if (foundNodes >= 50) {
		retValue = MAX_RETRIES_VALUE_THRESSHOLD_50MODULES;
	}	
	return retValue;
}