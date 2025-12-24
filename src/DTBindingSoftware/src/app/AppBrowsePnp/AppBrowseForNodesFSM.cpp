// BrowseForNodesObject.cpp: implementation of the CBrowseForNodesObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppBrowseForNodesFSM.h"
#include "NodeDatabase.h"		// "use"
#include "TXNodeMessage.h"		// "use"
#include "TXUnitMessage.h"		// "use"
#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
#define BROWSE_FOR_NODES_DEBUG
//#define DBASE_DEBUG_TRACE_ENABLE
//---------------------------------------------------------
#ifdef BROWSE_FOR_NODES_DEBUG
	#define BROWSE_FOR_NODES_TRACE(x)	TRACE(x)
#else

	#define BROWSE_FOR_NODES_TRACE(x)
#endif

//----------------------------------------------------------
#ifdef DBASE_DEBUG_TRACE_ENABLE
	#define DBASE_DEBUG_TRACE(x)			TRACE(x)
	#define DBASE_DEBUG_TRACE1(x,y)			TRACE1(x,y)
	#define DBASE_DEBUG_TRACE2(x,y,z)		TRACE2(x,y,z)
	#define DBASE_DEBUG_TRACE3(x,y,z,w)		TRACE3(x,y,z,w)
#else
	#define DBASE_DEBUG_TRACE(x)			
	#define DBASE_DEBUG_TRACE1(x,y)		
	#define DBASE_DEBUG_TRACE2(x,y,z)
	#define DBASE_DEBUG_TRACE3(x,y,z,w)		
#endif

//////////////////////////////////////////////////////////////////////
CBrowseForNodesObject::CBrowseForNodesObject(BYTE bAddress,IBrowsePnPEngine *pEngine,CNodeDatabase *pNodes) :
	m_pDatabase(pNodes), pBrowsePnpEngine(pEngine), pf_CurrentMode( 0 ),
	m_pNode(NULL), m_pUnit(NULL), pTxBrowseMessages(NULL), pTxUnitMessages(NULL), pTxBindingFileMessage(NULL)
{
	iNodeIndex=0;
	iUnitIndex=0;
	bNodeType=0;
	bNodeAddress= bAddress;
	this->setCurrentState(STATE_BROWSE_IDLE);

	m_iCurrentNROfNodes = 0;
	m_iPreviousNROfNodes = 0;

	pTxBrowseMessages = new CTXBrowsePnpMessages;
	pTxUnitMessages = new CTXUnitMessage;
	pTxBindingFileMessage = new CTXNodeBindingFileMessage;
}

CBrowseForNodesObject::CBrowseForNodesObject(IBrowsePnPEngine *pEngine,CNodeDatabase *pNodes) :
	m_pDatabase(pNodes), pBrowsePnpEngine(pEngine), pf_CurrentMode( 0 ),
	m_pNode(NULL), m_pUnit(NULL), pTxBrowseMessages(NULL), pTxUnitMessages(NULL), pTxBindingFileMessage(NULL)
{
	iNodeIndex=0;
	iUnitIndex=0;
	bNodeType= NODE_PIN_STD_UNITS|NODE_LCD|NODE_RESERVED|NODE_PIN_GATEWAY;
	bNodeAddress= BROADCAST_NODE;

	this->setCurrentState(STATE_BROWSE_IDLE);

	m_iCurrentNROfNodes = 0;
	m_iPreviousNROfNodes = 0;

	pTxBrowseMessages = new CTXBrowsePnpMessages();
	pTxUnitMessages = new CTXUnitMessage;
	pTxBindingFileMessage = new CTXNodeBindingFileMessage;

}

CBrowseForNodesObject::~CBrowseForNodesObject() {
	if (this->pTxBrowseMessages!=NULL) {
		delete this->pTxBrowseMessages;
		this->pTxBrowseMessages = NULL;
	}
	if (this->pTxUnitMessages != NULL) {
		delete this->pTxUnitMessages;
		this->pTxUnitMessages = NULL;
	}
	if (this->pTxBindingFileMessage != NULL) {
		delete this->pTxBindingFileMessage;
		this->pTxBindingFileMessage = NULL;
	}
}
///////////////////////////////////////////////////////////////
		
void CBrowseForNodesObject::BrowseAllNodes(int iEvent,int wParam,int lParam) {
	switch (this->m_CurrentState){
		case STATE_BROWSE_IDLE: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_BROWSE_IDLE ");
			switch (iEvent) {
				case EV_NODE_CONFIG:
		  		break;
				case EV_BROWSE_FOR_NODES: {					
					this->m_iCurrentNROfNodes = 0;
					this->m_iPreviousNROfNodes = 0;					
					const CString szMessage(_T("Browse for nodes..."));				
					CDisplayDevice::DisplayInfoMsg(szMessage);									
					this->pTxBrowseMessages->StartBrowse(this->bNodeType);					
					this->pBrowsePnpEngine->StartBrowseTimer(this->getTimerBroadcastReloadValue());	
					this->setCurrentState(STATE_BROWSE_NODES);							   
				} break; // EV_BROWSE_FOR_NODES
				case EV_BROWSE_CLOSE_FILE: {				   
				   this->setCurrentState(STATE_BROWSE_IDLE);
				} break; // EV_BROWSE_CLOSE_FILE
				case EV_BROWSE_USER_CANCEL: {					
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;				
				default: {
					const CString szMessage(_T("WARNING - Unknown event in STATE_BROWSE_IDLE NODES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);
				}break;
			} // switch(bEvent)
		} break; // STATE_BROWSE_IDLE

		case STATE_BROWSE_NODES: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_BROWSE_NODES ");			
			switch(iEvent) {
   				case EV_NODE_CONFIG:
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
		  		break;
   				case EV_BROADCAST_TIMEOUT: {
				   BROWSE_FOR_NODES_TRACE("--> EV_BROADCAST_TIMEOUT");
				   if (!this->m_pDatabase->NROfNodes(&this->m_iCurrentNROfNodes)) {
						// error
				   }
				   else {
					   	if (this->m_iPreviousNROfNodes != this->m_iCurrentNROfNodes) {
							CString szMessage;
							szMessage.Format("Browse for nodes again... Found %d logical nodes",m_iCurrentNROfNodes);						
							CDisplayDevice::DisplayInfoMsg(szMessage);
							this->m_iPreviousNROfNodes = this->m_iCurrentNROfNodes;
							this->pTxBrowseMessages->StartBrowse(this->bNodeType);							
							this->pBrowsePnpEngine->StartBrowseTimer(this->getTimerBroadcastReloadValue());													   
						}
						else {
							if (this->m_iCurrentNROfNodes == 0)  {
								const CString szMessage(_T("No nodes found or communication link not open, end browse."));
								CDisplayDevice::DisplayErrorMsg(szMessage);							
								this->setCurrentState(STATE_BROWSE_IDLE);
							}
							else {
								// Sedert V16.30: Extra controle doen of alle nodes correct zijn...
								// Na een physical browse for nodes moeten alle nodes antwoorden met hun nodetype.
								// Nog eens een extra controle doen op het node type.
								if (!checkNodeDatabaseValid( )) {								
									const CString szMessage("Browse for nodes again... Not all the nodes in the database have valid properties.");															
									CDisplayDevice::DisplayInfoMsg(szMessage);								
									this->pTxBrowseMessages->StartBrowse(bNodeType);							
									this->pBrowsePnpEngine->StartBrowseTimer(this->getTimerBroadcastReloadValue());									
								}
								else {
									// Sedert V16.45: De nodeconfig afzonderlijk opvragen per node.
									this->iNodeIndex = 0;
									this->m_pDatabase->GetNode(this->iNodeIndex,&this->m_pNode);		
									ASSERT(NULL != this->m_pNode);
									CString szMessage;
									szMessage.Format(_T("Get node 0x%02x config info."),this->m_pNode->bLogicalAddress);								
									CDisplayDevice::DisplayInfoMsg(szMessage);								
									this->pTxBrowseMessages->RequestNodeConfig(this->m_pNode->bLogicalAddress);
									this->pBrowsePnpEngine->StartPnPAckTimer();
									this->setCurrentState(STATE_GET_NODE_CONFIG);								 							
								}
							}
						}
				   }
				} break; // EV_BROWSE_FOR_NODES
						
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_END:
				case EV_UNIT_NAME_READ_ERROR:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
   				case EV_BROWSE_GET_UNIT_INFO: {
					BROWSE_FOR_NODES_TRACE("--> EVENT : DEFAULT");
					const CString szMessage(_T("Unknown event in STATE_BROWSE_FOR NODES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);			
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break; // EV_BROWSE_FOR_NODES

				case EV_BROWSE_CLOSE_FILE: {				
					  BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");   					
					  this->setCurrentState(STATE_BROWSE_IDLE);
					  this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break; // EV_BROWSE_CLOSE_FILE
				
				case EV_BROWSE_USER_CANCEL: {
					BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;

				case EV_BROWSE_NODEBINDING_INFO:
				case EV_BROWSE_FOR_NODES:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:	
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				default: {					
					const CString szMessage(_T("WARNING - Unknown event in STATE_BROWSE_FOR NODES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);
				} break;
     		} // switch(bEvent)
		} break; // STATE_BROWSE_NODES

		// Sedert V16.45: Voor iedere node afzonderlijk de node config opvragen.
		case STATE_GET_NODE_CONFIG: {
			switch(iEvent) {
				case EV_NORESPONSE: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("EV NORESPONSE");													
					const CString szMessage(_T("ERROR, No Response Received..."));				
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					this->pBrowsePnpEngine->Fire_OnBrowseTimeout();
				}break;						

				case EV_NODE_CONFIG: {					 		
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;					
					ASSERT(this->m_pNode != NULL);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						CString szMessage;
						szMessage.Format(_T("Get node 0x%02x name"), m_pNode->bLogicalAddress);
						CDisplayDevice::DisplayInfoMsg(szMessage);
						this->m_pNode->szNodeName[0] = '\0';
						this->pTxBrowseMessages->RequestNodeName(this->m_pNode->bLogicalAddress, strlen(this->m_pNode->szNodeName));
						this->pBrowsePnpEngine->StartPnPAckTimer();
						this->setCurrentState(STATE_GET_NODE_NAME);
					}
				}break;

				case EV_BROWSE_CLOSE_FILE: {
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(this->m_pNode != NULL);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");
						// TODO - CLOSE BINDINGFILE !!!
						//m_pNode->m_BindingFile.CloseFile();				
						const CString szMessage(_T("ERROR, try to browse again..."));
						CDisplayDevice::DisplayErrorMsg(szMessage);
						this->pBrowsePnpEngine->Fire_OnBrowseError();
					}
				} break;

				case EV_BROWSE_NODEBINDING_INFO:	
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_END:							
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4: {					
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(this->m_pNode != NULL);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						const CString szMessage(_T("Unknown event in STATE_GET_NODE_RESOURCES received..."));
						CDisplayDevice::DisplayErrorMsg(szMessage);
						this->setCurrentState(STATE_BROWSE_IDLE);
						this->pBrowsePnpEngine->Fire_OnBrowseError();
					}
				} break; // All others...

				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : DEFAULT");
					const CString szMessage(_T("Unknown event in STATE_GET_NODE_RESOURCES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break; // All others...

				case EV_BROWSE_USER_CANCEL: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;

				default:
				case EV_BROWSE_FOR_NODES:
				case EV_BROADCAST_TIMEOUT: {
					CString szMessage(_T("WARNING - Unknown event in STATE_GET_NODE_RESOURCES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);		
				}break;
			}	
		} break;

		case STATE_GET_NODE_NAME: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_NODE_NAME ");			
			switch(iEvent) {
				case EV_NORESPONSE: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("EV NORESPONSE");													
					const CString szMessage(_T("ERROR, No Response Received..."));				
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					this->pBrowsePnpEngine->Fire_OnBrowseTimeout();
				}break;
			
				case EV_NODE_CONFIG: {
					  BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				}break;

				case EV_BROWSE_CLOSE_FILE: {
					this->pBrowsePnpEngine->StopPnPAckTimer();						
					BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");
					// TODO - CLOSE BINDINGFILE !!!
					//m_pNode->m_BindingFile.CloseFile();				
					const CString szMessage(_T("ERROR, try to browse again..."));				
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break;

				case EV_NODE_NAME_READ_OK: {									
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(this->m_pNode != NULL);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();					
						this->pTxBrowseMessages->RequestNodeName(this->m_pNode->bLogicalAddress, strlen(this->m_pNode->szNodeName));
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_NODE_NAME_READ_OK

				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_END: {
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(this->m_pNode != NULL);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						pBrowsePnpEngine->StopPnPAckTimer();
						CString szMessage;
						szMessage.Format(_T("Get node 0x%02x resources..."), m_pNode->bLogicalAddress);
						CDisplayDevice::DisplayInfoMsg(szMessage);
						this->pTxBrowseMessages->RequestNodeResources(this->m_pNode->bLogicalAddress, 0);
						this->pBrowsePnpEngine->StartPnPAckTimer();
						this->setCurrentState(STATE_GET_NODE_RESOURCES);
					}
				} break; // EV_NODE_NAME_END || EV_NODE_NAME_READ_ERROR
					
				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					const CString szMessage(_T("Unknown event in STATE_GET_NODE_NAME received..."));						
					CDisplayDevice::DisplayErrorMsg(szMessage);
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break; // All others...

				case EV_BROWSE_USER_CANCEL: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;

				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				case EV_BROWSE_NODEBINDING_INFO: {
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(this->m_pNode != NULL);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						CString szMessage(_T("WARNING - Unknown event in STATE_GET_NODE_NAME received..."));
						CDisplayDevice::DisplayErrorMsg(szMessage);
					}
				} break;

				default:				
				case EV_BROWSE_FOR_NODES:
				case EV_BROADCAST_TIMEOUT: {
					CString szMessage(_T("WARNING - Unknown event in STATE_GET_NODE_NAME received..."));						
					CDisplayDevice::DisplayErrorMsg(szMessage);
				}break;
			 } // switch(iEvent)
		} break; // STATE_GET_NODE_NAME

		case STATE_GET_NODE_RESOURCES: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_NODE_RESOURCES ");
			switch(iEvent) {
				case EV_NORESPONSE: {				
					pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("EV NORESPONSE");													
					CString szMessage(_T("ERROR, No Response Received..."));				
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					pBrowsePnpEngine->Fire_OnBrowseTimeout();
				}break;
						
				case EV_NODE_CONFIG: {					 
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				}break;

				case EV_BROWSE_CLOSE_FILE: {				  
					pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");								
					CString szMessage(_T("ERROR, try to browse again..."));			
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					pBrowsePnpEngine->Fire_OnBrowseError();
				} break;

				case EV_NODE_RESOURCES0: {					
					ASSERT(NULL != this->m_pNode);
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (this->m_pNode->getNodeAddress() == bNodeAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();					
						this->pTxBrowseMessages->RequestNodeResources(m_pNode->bLogicalAddress, 1);
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_NODE_RESOURCES_0

				case EV_NODE_RESOURCES1: {        			
					ASSERT(NULL != this->m_pNode);
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (this->m_pNode->getNodeAddress() == bNodeAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();		
						this->pTxBrowseMessages->RequestNodeResources(m_pNode->bLogicalAddress, 2);
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_NODE_RESOURCES_1

				case EV_NODE_RESOURCES2: {				
					ASSERT(NULL != this->m_pNode);
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (this->m_pNode->getNodeAddress() == bNodeAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();			
						// Sedert versie 0x0700.
						// Bepalen of er extra resource blocks moeten opgevraagd worden...
						// Sedert versie V16.32:
						// Opvragen van de node binding file info in een andere toestand.
						if (m_pNode->GetExtraResourceBlocks() == 0) {
							this->pTxBindingFileMessage->RequestBindingInfoFile(m_pNode->bLogicalAddress);
							this->setCurrentState(STATE_GET_NODE_BINDINGFILEINFO);
						}
						else {
							this->pTxBrowseMessages->RequestNodeResources(m_pNode->bLogicalAddress, 3);
						}
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				}break;

				// Sedert versie 0x0700.
				// Oppassen: Vragen momenteel maar maximum 2 extra resource blocks.
				// Kunnen dit evt later algemeen principe doen.
				// Sedert versie V16.32:
				// Opvragen van de node binding file info in een andere toestand.
				case EV_NODE_RESOURCES3: {
					ASSERT(NULL != this->m_pNode);
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (this->m_pNode->getNodeAddress() == bNodeAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						if (m_pNode->GetExtraResourceBlocks() <= 1) {
							this->pTxBindingFileMessage->RequestBindingInfoFile(m_pNode->bLogicalAddress);
							this->setCurrentState(STATE_GET_NODE_BINDINGFILEINFO);
						}
						else {
							this->pTxBrowseMessages->RequestNodeResources(m_pNode->bLogicalAddress, 4);
						}
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break;

				// Sedert versie 0x0700.
				// Sedert versie V16.32:
				// Opvragen van de node binding file info in een andere toestand.
				case EV_NODE_RESOURCES4: {
					ASSERT(NULL != this->m_pNode);
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (this->m_pNode->getNodeAddress() == bNodeAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();					
						this->pTxBindingFileMessage->RequestBindingInfoFile(m_pNode->bLogicalAddress);
						this->setCurrentState(STATE_GET_NODE_BINDINGFILEINFO);
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break;

				case EV_BROWSE_NODEBINDING_INFO:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_END: {					
					ASSERT(NULL != this->m_pNode);
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (this->m_pNode->getNodeAddress() == bNodeAddress)) {
						pBrowsePnpEngine->StopPnPAckTimer();						
						const CString szMessage(_T("Unknown event in STATE_GET_NODE_RESOURCES received..."));
						CDisplayDevice::DisplayErrorMsg(szMessage);
						this->setCurrentState(STATE_BROWSE_IDLE);
						this->pBrowsePnpEngine->Fire_OnBrowseError();
					}
				} break; // All others...
			
				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR: {
					pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : DEFAULT");
					CString szMessage(_T("Unknown event in STATE_GET_NODE_RESOURCES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);
					this->setCurrentState(STATE_BROWSE_IDLE);
					pBrowsePnpEngine->Fire_OnBrowseError();
				} break; // All others...

				case EV_BROWSE_USER_CANCEL: {				
					pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
					this->setCurrentState(STATE_BROWSE_IDLE);
					pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;

				default:
				case EV_BROWSE_FOR_NODES:
				case EV_BROADCAST_TIMEOUT: {
					CString szMessage(_T("WARNING - Unknown event in STATE_GET_NODE_RESOURCES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);		
				}break;

			 } // switch(iEvent)
		} break; // STATE_GET_NODE_RESOURCES

		// Sedert versie V16.32:
		// Opvragen van de node binding file info in een andere toestand.
		case STATE_GET_NODE_BINDINGFILEINFO: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_NODE_BINDINGFILEINFO ");
			switch(iEvent) {
				case EV_NORESPONSE: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("EV NORESPONSE");													
					const CString szMessage(_T("ERROR, No Response Received..."));				
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					this->pBrowsePnpEngine->Fire_OnBrowseTimeout();
				}break;

				case EV_NODE_CONFIG: {					 
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				}break;

				case EV_BROWSE_CLOSE_FILE: {				  
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");								
					const CString szMessage(_T("ERROR, try to browse again..."));			
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break;

				case EV_BROWSE_NODEBINDING_INFO: {						
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						int iNROfNodes = 0;
						this->pBrowsePnpEngine->StopPnPAckTimer();					
						this->iNodeIndex++;
						if (!this->m_pDatabase->NROfNodes(&iNROfNodes)) ASSERT(0); //ERROR
						if (this->iNodeIndex < iNROfNodes) {						
							if (this->m_pDatabase->GetNode(this->iNodeIndex, &this->m_pNode)) {															
								CString szMessage;
								szMessage.Format(_T("Get node 0x%02x config info."), this->m_pNode->bLogicalAddress);
								CDisplayDevice::DisplayInfoMsg(szMessage);
								this->pTxBrowseMessages->RequestNodeConfig(this->m_pNode->bLogicalAddress);
								this->pBrowsePnpEngine->StartPnPAckTimer();
								this->setCurrentState(STATE_GET_NODE_CONFIG);
							}
							else {							
								const CString szMessage(_T("Node not found, end browse."));
								CDisplayDevice::DisplayErrorMsg(szMessage);
								this->setCurrentState(STATE_BROWSE_IDLE);
								this->pBrowsePnpEngine->Fire_OnBrowseError();
							}
						}
						else {
							this->iNodeIndex = 0;
							this->iUnitIndex = 0;
							while (1) {
								if (!m_pDatabase->GetNode(this->iNodeIndex, &this->m_pNode)) {									
									const CString szMessage(_T("Node not found, end browse."));
									CDisplayDevice::DisplayErrorMsg(szMessage);
									this->setCurrentState(STATE_BROWSE_IDLE);
									this->pBrowsePnpEngine->Fire_OnBrowseError();
									break;
								}
								else {
									if ((this->m_pNode->bNROfUnits != 0) && (this->m_pNode->bNodeType & bNodeType)) {
										this->iUnitIndex = 0;
										this->m_pUnit = NULL;
										// this->m_pDatabase->GetUnit(this->m_pNode, this->iUnitIndex, &this->m_pUnit);
										// ASSERT(NULL != this->m_pUnit);	OPPASSEN this->m_pUnit = NULL.
										CString szMessage;
										szMessage.Format(_T("Get unit 0x%02x information of node 0x%02x"), this->iUnitIndex, this->m_pNode->bLogicalAddress);
										CDisplayDevice::DisplayInfoMsg(szMessage);
										this->pTxUnitMessages->RequestConfig(this->m_pNode->bLogicalAddress, this->iUnitIndex);
										this->pBrowsePnpEngine->StartPnPAckTimer();
										this->setCurrentState(STATE_GET_UNIT_INFO);
										break;
									}
									else {
										if (this->iNodeIndex++ >= (iNROfNodes - 1)) {									
											const CString szMessage(_T("No more nodes found with units, end browse."));
											CDisplayDevice::DisplayErrorMsg(szMessage);
											this->setCurrentState(STATE_BROWSE_IDLE);
											this->pBrowsePnpEngine->Fire_OnBrowseError();
											break;
										}
									}
								}
							} // while(1)
						}
					}
				} break; // EV_BROWSE_NODEBINDING_INFO

				case EV_NODE_RESOURCES0:				
				case EV_NODE_RESOURCES1:				
				case EV_NODE_RESOURCES2:					
				case EV_NODE_RESOURCES3:					
				case EV_NODE_RESOURCES4:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_END: {			
					ASSERT(wParam >= 0 && wParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						BROWSE_FOR_NODES_TRACE("--> EVENT : DEFAULT");
						const CString szMessage(_T("Unknown event in STATE_GET_NODE_RESOURCES received..."));
						CDisplayDevice::DisplayErrorMsg(szMessage);
						this->setCurrentState(STATE_BROWSE_IDLE);
						this->pBrowsePnpEngine->Fire_OnBrowseError();
					}
				} break; // All others...

				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : DEFAULT");
					const CString szMessage(_T("Unknown event in STATE_GET_NODE_RESOURCES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseError();					
				} break; // All others...

				case EV_BROWSE_USER_CANCEL: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;

				default:
				case EV_BROWSE_FOR_NODES:
				case EV_BROADCAST_TIMEOUT: {
					CString szMessage(_T("WARNING - Unknown event in STATE_GET_NODE_RESOURCES received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);		
				}break;			
			} /* end switch */											 
		} break;

		case STATE_GET_UNIT_NAME: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_UNIT_NAME ");			
			switch(iEvent) {
				case EV_NORESPONSE: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("EV NORESPONSE");													
					const CString szMessage(_T("ERROR, No Response Received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);									
					this->pBrowsePnpEngine->Fire_OnBrowseTimeout();
				}break;
						
				case EV_NODE_CONFIG: {					
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				} break;

				case EV_BROWSE_CLOSE_FILE:
				{				  
					this->pBrowsePnpEngine->StopPnPAckTimer();					
					BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");					
					// TODO - CLOSE BINDINGFILE !!!
					//m_pNode->m_BindingFile.CloseFile();					
					const CString szMessage(_T("ERROR, try to browse again..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					pBrowsePnpEngine->Fire_OnBrowseError();
				} break;

				case EV_UNIT_NAME_READ_OK: {					
					ASSERT(wParam >= 0 && wParam <= 0xff);
					ASSERT(lParam >= 0 && lParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					const BYTE bUnitAddress = (BYTE)lParam;
					if ((NULL != this->m_pNode) && (NULL != this->m_pUnit) && (bNodeAddress == this->m_pNode->bLogicalAddress) && (bUnitAddress == this->m_pUnit->bUnitAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();						
						this->pTxUnitMessages->RequestUnitName(this->m_pNode->bLogicalAddress, this->iUnitIndex, strlen(this->m_pUnit->szUnitName));
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_UNIT_NAME_READ_OK

				case EV_UNIT_NAME_READ_ERROR:
				case EV_UNIT_NAME_END: {					
					ASSERT(wParam >= 0 && wParam <= 0xff);
					ASSERT(lParam >= 0 && lParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					const BYTE bUnitAddress = (BYTE)lParam;
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->bLogicalAddress) && (bUnitAddress == this->iUnitIndex)) {
						BROWSE_FOR_NODES_TRACE("--> EV_UNIT_NAME_READ_ERROR + EV_UNIT_NAME_END");
						DBASE_DEBUG_TRACE3("\n---Node:0x%02x,Unit:0x%02x,'%s'---\n", this->m_pNode->bLogicalAddress, this->m_pUnit->bUnitAddress, &this->m_pUnit->szUnitName[0]);
						this->pBrowsePnpEngine->StopPnPAckTimer();
						this->iUnitIndex++;
						if (this->iUnitIndex < (int)this->m_pNode->bNROfUnits) {
							CString szMessage;
							szMessage.Format(_T("Get unit 0x%02x information of node 0x%02x"), this->iUnitIndex, this->m_pNode->bLogicalAddress);
							CDisplayDevice::DisplayInfoMsg(szMessage);
							this->m_pUnit = NULL;
							this->pTxUnitMessages->RequestConfig(this->m_pNode->bLogicalAddress, this->iUnitIndex);
							this->pBrowsePnpEngine->StartPnPAckTimer();
							this->setCurrentState(STATE_GET_UNIT_INFO);
						}
						else {
							while (1) {
								int iNROfNodes = 0;
								if (!this->m_pDatabase->NROfNodes(&iNROfNodes)) ASSERT(0); //ERROR													
								if (this->iNodeIndex++ >= (iNROfNodes - 1)) {
									this->setCurrentState(STATE_BROWSE_IDLE);
									this->pBrowsePnpEngine->Fire_OnBrowseDone();
									break;
								}
								else {								
									if (this->m_pDatabase->GetNode(this->iNodeIndex, &this->m_pNode)) {
										if ((this->m_pNode->bNROfUnits != 0) && (this->m_pNode->bNodeType & bNodeType)) {
											this->iUnitIndex = 0;
											this->m_pUnit = NULL;
											CString szMessage;
											szMessage.Format(_T("Get unit 0x%02x information of node 0x%02x"), this->iUnitIndex, this->m_pNode->bLogicalAddress);
											CDisplayDevice::DisplayInfoMsg(szMessage);
											this->pTxUnitMessages->RequestConfig(this->m_pNode->bLogicalAddress, this->iUnitIndex);
											this->pBrowsePnpEngine->StartPnPAckTimer();
											this->setCurrentState(STATE_GET_UNIT_INFO);
											break;
										}
									}
								}
							} // while(1)
						}
					}
				} break; // EV_UNIT_NAME_END || EV_UNIT_NAME_READ_ERROR
			
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_END:
				case EV_BROWSE_GET_UNIT_INFO: {				  
					pBrowsePnpEngine->StopPnPAckTimer();
					
					BROWSE_FOR_NODES_TRACE("--> EVENT : DEFAULT");					
					CString szMessage(_T("Unknown event in STATE_GET_UNIT_NAME received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);

					this->setCurrentState(STATE_BROWSE_IDLE);
					pBrowsePnpEngine->Fire_OnBrowseError();
				}break;
		
				case EV_BROWSE_USER_CANCEL: {				
					pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
					this->setCurrentState(STATE_BROWSE_IDLE);
					pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;

				case EV_BROWSE_FOR_NODES:
   				case EV_BROADCAST_TIMEOUT:
				case EV_BROWSE_NODEBINDING_INFO:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				default: {
					CString szMessage(_T("WARNING - Unknown event in STATE_GET_UNITNAME received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);		
				}break;
			 } // switch(iEvent)
		  } break; // STATE_GET_UNIT_NAME

		  case STATE_GET_UNIT_INFO: {			
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_UNIT_INFO ");			  
			switch(iEvent) {
				case EV_NORESPONSE: {					
					this->pBrowsePnpEngine->StopPnPAckTimer();
					BROWSE_FOR_NODES_TRACE("EV NORESPONSE");					
					const CString szMessage(_T("ERROR, No Response Received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					this->pBrowsePnpEngine->Fire_OnBrowseTimeout();
				}break;
			
				case EV_NODE_CONFIG: {					
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				} break;

				case EV_BROWSE_CLOSE_FILE: {				  
					this->pBrowsePnpEngine->StopPnPAckTimer();					
					BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");					
					// TODO - CLOSE BINDINGFILE !!!
					//m_pNode->m_BindingFile.CloseFile();					
					const CString szMessage(_T("ERROR, try to browse again..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);					
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break;

   				case EV_BROWSE_GET_UNIT_INFO: {  				
					ASSERT(wParam >= 0 && wParam <= 0xff);
					ASSERT(lParam >= 0 && lParam <= 0xff);
					const BYTE bNodeAddress = (BYTE)wParam;
					const BYTE bUnitAddress = (BYTE)lParam;
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->bLogicalAddress) && (bUnitAddress == this->iUnitIndex)) {
						// TM 20080311: Controleren of de pUnit kan gevonden worden in de database. (Bug: pUnit was 0 !!!)
						if (m_pDatabase->GetUnit(this->m_pNode, this->iUnitIndex, &this->m_pUnit)) {
							this->pBrowsePnpEngine->StopPnPAckTimer();						
							CString szMessage;
							szMessage.Format(_T("Get unit 0x%02x name of node 0x%02x"), this->iUnitIndex, this->m_pNode->bLogicalAddress);
							CDisplayDevice::DisplayInfoMsg(szMessage);
							this->m_pUnit->szUnitName[0] = '\0';
							this->pTxUnitMessages->RequestUnitName(this->m_pNode->bLogicalAddress, this->iUnitIndex, strlen(this->m_pUnit->szUnitName));
							this->pBrowsePnpEngine->StartPnPAckTimer();
							this->setCurrentState(STATE_GET_UNIT_NAME);
						}
						else {
							this->pBrowsePnpEngine->StopPnPAckTimer();
							const CString szMessage(_T("Message from wrong unit received..."));
							CDisplayDevice::DisplayErrorMsg(szMessage);
							this->setCurrentState(STATE_BROWSE_IDLE);
							this->pBrowsePnpEngine->Fire_OnBrowseError();
						}
					}
				} break; //EV_BROWSE_GET_UNIT_INFO
			
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_END: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();					
					BROWSE_FOR_NODES_TRACE("--> EVENT : DEFAULT");
			  		const CString szMessage(_T("Unknown event in STATE_GET_UNIT_INFO received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				}break;

				case EV_UNIT_NAME_READ_ERROR:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END: {				
					this->pBrowsePnpEngine->StopPnPAckTimer();					
					BROWSE_FOR_NODES_TRACE("--> EVENT_UNIT_NAME_ERROR + OK + END");
			  		const CString szMessage(_T("WARNING - Unknown event in STATE_GET_UNIT_INFO received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);	
				}break;

				case EV_BROWSE_USER_CANCEL: {
					this->pBrowsePnpEngine->StopPnPAckTimer();					
					BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
					this->setCurrentState(STATE_BROWSE_IDLE);
					this->pBrowsePnpEngine->Fire_OnBrowseCancel();
				}break;

				case EV_BROWSE_FOR_NODES:
   				case EV_BROADCAST_TIMEOUT:
				case EV_BROWSE_NODEBINDING_INFO:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				default: {
					const CString szMessage(_T("WARNING - Unknown event in STATE_GET_UNIT_INFO received..."));
					CDisplayDevice::DisplayErrorMsg(szMessage);
				}break;
			 } // switch(iEvent)
		  } break; // STATE_GET_UNIT_INFO
	}// SWITCH CURRENTSTATE
}

void CBrowseForNodesObject::BrowseSingleNode(int iEvent,int wParam,int lParam) {	
	switch (this->m_CurrentState) {
		case STATE_BROWSE_IDLE: {
			switch (iEvent) {
				case EV_BROWSE_FOR_NODES: {
					/* Start Browse Single node */
					CString szMessage;
					szMessage.Format (_T("Get node 0x%02x config"),this->bNodeAddress);
					CDisplayDevice::DisplayInfoMsg(szMessage);
					this->pBrowsePnpEngine->StartPnPAckTimer();
					this->pTxBrowseMessages->RequestNodeConfig(this->bNodeAddress);
					this->setCurrentState(STATE_BROWSE_NODES);
				} break;
				case EV_BROWSE_USER_CANCEL: {
					this->helper_handleUserCancel( );
				}break;
				case EV_BROADCAST_TIMEOUT:
				case EV_BROWSE_GET_UNIT_INFO:				
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_END:
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				case EV_BROWSE_CLOSE_FILE:
				case EV_NODE_CONFIG:
				case EV_BROWSE_NODEBINDING_INFO:	
					// absorbe...
				break;
			} // end switch (iEvent)
		}break; // End State Browse Idle 

		case STATE_BROWSE_NODES: {
			switch (iEvent) {
				case EV_NODE_CONFIG: {
					this->iNodeIndex = 0;                    
					this->m_pNode = NULL;
					int iEntryNR;
					if (this->m_pDatabase->FindNode(this->bNodeAddress,&iEntryNR,&this->m_pNode)) {					
						ASSERT(NULL != this->m_pNode);
						this->pBrowsePnpEngine->StopPnPAckTimer();						
						this->m_pNode->szNodeName[0] = '\0';
						this->pTxBrowseMessages->RequestNodeName(this->bNodeAddress,strlen(this->m_pNode->szNodeName));		
						this->pBrowsePnpEngine->StartPnPAckTimer();	
						this->setCurrentState(STATE_GET_NODE_NAME);
					}
					else {
						// TODO: Wat doen: Timer expiry wachten?
						ASSERT(0);
					}
				}break; 
				case EV_BROWSE_CLOSE_FILE: {
					this->setCurrentState(STATE_BROWSE_IDLE);	
					this->pBrowsePnpEngine->Fire_OnBrowseError();
				} break;
				case EV_NORESPONSE: {
					this->helper_handleNoResponse( );
				} break;
				case EV_BROWSE_FOR_NODES:
				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_END:
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				case EV_BROWSE_NODEBINDING_INFO: {				
					this->helper_handleWrongResponse();
				} break;
				case EV_BROWSE_USER_CANCEL: {
					this->helper_handleUserCancel( );
				}break;
			} // end switch (iEvent)
		}break;// end state Browse nodes

		case STATE_GET_NODE_NAME: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_NODE_NAME ");			
			switch(iEvent) {
				case EV_NODE_NAME_READ_OK: {				
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT((NULL != this->m_pNode) && (NULL != m_pNode->szNodeName));					
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						this->pTxBrowseMessages->RequestNodeName(m_pNode->bLogicalAddress, strlen(m_pNode->szNodeName));
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_NODE_NAME_READ_OK

				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_END: {
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(NULL != this->m_pNode);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {					
						this->pBrowsePnpEngine->StopPnPAckTimer();
						CString szMessage;
						szMessage.Format(_T("Get node 0x%02x resources..."), m_pNode->bLogicalAddress);
						CDisplayDevice::DisplayInfoMsg(szMessage);
						this->pTxBrowseMessages->RequestNodeResources(this->m_pNode->bLogicalAddress, 0);
						this->pBrowsePnpEngine->StartPnPAckTimer();
						this->setCurrentState(STATE_GET_NODE_RESOURCES);
					}
				} break; // EV_NODE_NAME_END || EV_NODE_NAME_READ_ERROR
				case EV_NORESPONSE: {				
					this->helper_handleNoResponse( );					
				}break;			
				case EV_NODE_CONFIG: {
					  BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				}break;
				case EV_BROWSE_CLOSE_FILE: {
					this->helper_handleResponseCloseFile( );
				} break;									
				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR: {
					this->helper_handleWrongResponse();
				} break; // All others...
				case EV_BROWSE_USER_CANCEL: {				
					this->helper_handleUserCancel( );
				}break;
				default:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				case EV_BROWSE_FOR_NODES:
				case EV_BROADCAST_TIMEOUT:
				case EV_BROWSE_NODEBINDING_INFO: {
					this->helper_displayWrongEventReceived( );
				}break;
			 } // switch(iEvent)
		} break; // STATE_GET_NODE_NAME

		case STATE_GET_NODE_RESOURCES: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_NODE_RESOURCES ");
			switch(iEvent) {		
				case EV_NODE_RESOURCES0: {										
					ASSERT((NULL != this->m_pNode) && (this->m_pNode->bLogicalAddress == this->bNodeAddress));
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(NULL != this->m_pNode);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						this->pTxBrowseMessages->RequestNodeResources(this->m_pNode->bLogicalAddress, 1);
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_NODE_RESOURCES_0

				case EV_NODE_RESOURCES1: {        							
					ASSERT((NULL != this->m_pNode) && (this->m_pNode->bLogicalAddress == this->bNodeAddress));
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(NULL != this->m_pNode);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						this->pTxBrowseMessages->RequestNodeResources(this->m_pNode->bLogicalAddress, 2);
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_NODE_RESOURCES_1

				case EV_NODE_RESOURCES2: {								
					ASSERT((NULL != this->m_pNode) && (this->m_pNode->bLogicalAddress == this->bNodeAddress));
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(NULL != this->m_pNode);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						// Sedert versie 0x0700.
						// Bepalen of er extra resource blocks moeten opgevraagd worden...
						// Sedert versie V16.32:
						// Opvragen van de node binding file info in een andere toestand.
						if (this->m_pNode->GetExtraResourceBlocks() == 0) {
							this->pTxBindingFileMessage->RequestBindingInfoFile(this->m_pNode->bLogicalAddress);
							this->setCurrentState(STATE_GET_NODE_BINDINGFILEINFO);
						}
						else {
							this->pTxBrowseMessages->RequestNodeResources(m_pNode->bLogicalAddress, 3);
						}
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				}break;

				// Sedert versie 0x0700.
				// Oppassen: Vragen momenteel maar maximum 2 extra resource blocks.
				// Kunnen dit evt later algemeen principe doen.
				// Sedert versie V16.32:
				// Opvragen van de node binding file info in een andere toestand.
				case EV_NODE_RESOURCES3: {					
					ASSERT((NULL != this->m_pNode) && (this->m_pNode->bLogicalAddress == this->bNodeAddress));
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(NULL != this->m_pNode);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						if (this->m_pNode->GetExtraResourceBlocks() <= 1) {
							this->pTxBindingFileMessage->RequestBindingInfoFile(this->m_pNode->bLogicalAddress);
							this->setCurrentState(STATE_GET_NODE_BINDINGFILEINFO);
						}
						else {
							this->pTxBrowseMessages->RequestNodeResources(m_pNode->bLogicalAddress, 4);
						}
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break;

				// Sedert versie 0x0700.
				// Sedert versie V16.32:
				// Opvragen van de node binding file info in een andere toestand.
				case EV_NODE_RESOURCES4: {
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_RESOURCES4");		
					ASSERT((NULL != this->m_pNode) && (this->m_pNode->bLogicalAddress == this->bNodeAddress));
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(NULL != this->m_pNode);
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress())) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						this->pTxBindingFileMessage->RequestBindingInfoFile(this->m_pNode->bLogicalAddress);
						this->setCurrentState(STATE_GET_NODE_BINDINGFILEINFO);
						pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break;
				case EV_NORESPONSE: {				
					this->helper_handleNoResponse( );
				}break;					
				case EV_NODE_CONFIG: {					 
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				}break;
				case EV_BROWSE_CLOSE_FILE: {				  
					this->helper_handleResponseCloseFile( );
				} break;
				case EV_BROWSE_NODEBINDING_INFO:	// sedert V16.32: Afhandelen in een andere toestand.
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_END:			
				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR: {	
					this->helper_handleWrongResponse();
				} break; 
				case EV_BROWSE_USER_CANCEL: {				
					this->helper_handleUserCancel( );
				}break;
				default:
				case EV_BROWSE_FOR_NODES:
				case EV_BROADCAST_TIMEOUT: {
					this->helper_displayWrongEventReceived( );		
				}break;

			 } // switch(iEvent)
		} break; // STATE_GET_NODE_RESOURCES

		// Sedert versie V16.32:
		// Opvragen van de node binding file info in een andere toestand.
		case STATE_GET_NODE_BINDINGFILEINFO: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_NODE_BINDINGFILEINFO ");
			switch(iEvent) {
				case EV_BROWSE_NODEBINDING_INFO: {			
					BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_NODEBINDING_INFO");
					bool continueBrowseUnits = false;
					this->pBrowsePnpEngine->StopPnPAckTimer();			
					this->iNodeIndex = 0;
					this->iUnitIndex = 0;				  						
					if (this->m_pNode->bNROfUnits>0) {
						this->iUnitIndex=0;					
						this->m_pUnit = NULL;
						if (this->m_pDatabase->GetUnit(this->m_pNode,this->iUnitIndex,&this->m_pUnit)) {							
							CString szMessage;
							szMessage.Format(_T("Get unit 0x%02x information of node 0x%02x"),iUnitIndex,m_pNode->bLogicalAddress);
							CDisplayDevice::DisplayInfoMsg(szMessage);															
							this->pTxUnitMessages->RequestConfig(this->m_pNode->bLogicalAddress,this->iUnitIndex);
							this->pBrowsePnpEngine->StartPnPAckTimer();
							continueBrowseUnits = true;
							this->setCurrentState(STATE_GET_UNIT_INFO);																											
						}						
					}	
					if (false == continueBrowseUnits) {
						this->pBrowsePnpEngine->StopPnPAckTimer();												
						const CString szMessage(_T("Error: The node holds no units."));			
						CDisplayDevice::DisplayErrorMsg(szMessage);					
						this->pBrowsePnpEngine->Fire_OnBrowseError();
					}
				} break; // EV_BROWSE_NODEBINDING_INFO
				case EV_NORESPONSE: {				
					this->helper_handleNoResponse( );
				}break;
				case EV_NODE_CONFIG: {					 
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				}break;
				case EV_BROWSE_CLOSE_FILE: {				  
					this->helper_handleResponseCloseFile();
				} break;
				case EV_NODE_RESOURCES0:				
				case EV_NODE_RESOURCES1:				
				case EV_NODE_RESOURCES2:					
				case EV_NODE_RESOURCES3:					
				case EV_NODE_RESOURCES4:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_END:			
				case EV_BROWSE_GET_UNIT_INFO:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END:
				case EV_UNIT_NAME_READ_ERROR: {					
					this->helper_handleWrongResponse();
				} break; // All others...
				case EV_BROWSE_USER_CANCEL: {				
					this->helper_handleUserCancel( );
				}break;
				default:
				case EV_BROWSE_FOR_NODES:
				case EV_BROADCAST_TIMEOUT: {
					this->helper_displayWrongEventReceived( );		
				}break;			
			} /* end switch */											 
		} break;

		case STATE_GET_UNIT_INFO: {			
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_UNIT_INFO ");			  
			switch(iEvent) {
				case EV_BROWSE_GET_UNIT_INFO: {  				
					ASSERT(NULL != this->m_pNode);
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(lParam >= 0 && lParam <= 0xFF);
					const BYTE bUnitAddress = (BYTE)lParam;
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress()) && (this->iUnitIndex == bUnitAddress)) {
						this->m_pUnit = NULL;
						if (this->m_pDatabase->GetUnit(this->m_pNode, this->iUnitIndex, &this->m_pUnit)) {
							BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_GET_UNIT_INFO");
							this->pBrowsePnpEngine->StopPnPAckTimer();
							CString szMessage;
							szMessage.Format(_T("Get unit 0x%02x name of node 0x%02x"), this->iUnitIndex, this->m_pNode->bLogicalAddress);
							CDisplayDevice::DisplayInfoMsg(szMessage);

							this->m_pUnit->szUnitName[0] = '\0';
							this->pTxUnitMessages->RequestUnitName(this->m_pNode->bLogicalAddress, this->iUnitIndex, strlen(this->m_pUnit->szUnitName));
							this->pBrowsePnpEngine->StartPnPAckTimer();
							this->setCurrentState(STATE_GET_UNIT_NAME);
						}
						else {
							this->pBrowsePnpEngine->StopPnPAckTimer();
							const CString szMessage(_T("Message from wrong unit received..."));
							CDisplayDevice::DisplayErrorMsg(szMessage);
							this->setCurrentState(STATE_BROWSE_IDLE);
							this->pBrowsePnpEngine->Fire_OnBrowseError();
						}
					}
				} break; //EV_BROWSE_GET_UNIT_INFO

				case EV_NORESPONSE: {	
					this->helper_handleNoResponse( );
				}break;			
				case EV_NODE_CONFIG: {					
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				} break;
				case EV_BROWSE_CLOSE_FILE: {				  
					this->helper_handleResponseCloseFile( );
				} break;
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_END:	{				
					this->helper_handleWrongResponse();
				}break;
				case EV_UNIT_NAME_READ_ERROR:
				case EV_UNIT_NAME_READ_OK:
				case EV_UNIT_NAME_END: {				
					BROWSE_FOR_NODES_TRACE("--> EVENT_UNIT_NAME_ERROR + OK + END");				
			  		this->helper_displayWrongEventReceived( );		
				}break;

				case EV_BROWSE_USER_CANCEL: {
					this->helper_handleUserCancel( );
				}break;
				case EV_BROWSE_FOR_NODES:
   				case EV_BROADCAST_TIMEOUT:
				case EV_BROWSE_NODEBINDING_INFO:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				default: {
					this->helper_displayWrongEventReceived( );		
				}break;
			} /* end switch */
		} break; /* STATE_GET_UNIT_INFO */
   			
		case STATE_GET_UNIT_NAME: {
			BROWSE_FOR_NODES_TRACE("\nSTATE_GET_UNIT_NAME ");			
			switch(iEvent) {
				case EV_UNIT_NAME_READ_OK: {					
					BROWSE_FOR_NODES_TRACE("--> EV_UNIT_NAME_READ_OK");
					ASSERT(NULL != this->m_pNode);
					ASSERT(NULL != this->m_pUnit);
					ASSERT(NULL != this->m_pUnit->szUnitName);				
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(lParam >= 0 && lParam <= 0xFF);
					const BYTE bUnitAddress = (BYTE)lParam;
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress()) && (this->iUnitIndex == bUnitAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						this->pTxUnitMessages->RequestUnitName(this->m_pNode->bLogicalAddress, this->iUnitIndex, strlen(this->m_pUnit->szUnitName));
						this->pBrowsePnpEngine->StartPnPAckTimer();
					}
				} break; // EV_UNIT_NAME_READ_OK

				case EV_UNIT_NAME_READ_ERROR:
				case EV_UNIT_NAME_END: {					
					BROWSE_FOR_NODES_TRACE("--> EV_UNIT_NAME_READ_ERROR + EV_UNIT_NAME_END");
					ASSERT(NULL != this->m_pNode);
					ASSERT(NULL != this->m_pUnit);
					ASSERT(NULL != this->m_pUnit->szUnitName);
					ASSERT(wParam >= 0 && wParam <= 0xFF);
					const BYTE bNodeAddress = (BYTE)wParam;
					ASSERT(lParam >= 0 && lParam <= 0xFF);
					const BYTE bUnitAddress = (BYTE)lParam;
					if ((NULL != this->m_pNode) && (bNodeAddress == this->m_pNode->getNodeAddress()) && (this->iUnitIndex == bUnitAddress)) {
						this->pBrowsePnpEngine->StopPnPAckTimer();
						DBASE_DEBUG_TRACE3("\n---Node:0x%02x,Unit:0x%02x,'%s'---\n", m_pNode->bLogicalAddress, m_pUnit->bUnitAddress, &m_pUnit->szUnitName[0]);
						this->iUnitIndex++;
						if (this->iUnitIndex < (int)m_pNode->bNROfUnits) {
							CString szMessage;
							szMessage.Format(_T("Get unit 0x%02x information of node 0x%02x"), this->iUnitIndex, this->m_pNode->bLogicalAddress);
							CDisplayDevice::DisplayInfoMsg(szMessage);
							this->pTxUnitMessages->RequestConfig(this->m_pNode->bLogicalAddress, this->iUnitIndex);
							this->pBrowsePnpEngine->StartPnPAckTimer();
							this->setCurrentState(STATE_GET_UNIT_INFO);
						}
						else {
							const CString szMessage(_T("Request node done."));
							CDisplayDevice::DisplayInfoMsg(szMessage);
							this->setCurrentState(STATE_BROWSE_IDLE);
							this->pBrowsePnpEngine->Fire_OnBrowseDone();
						}
					}
				} break; // EV_UNIT_NAME_END || EV_UNIT_NAME_READ_ERROR

				case EV_NORESPONSE: {				
					this->helper_handleNoResponse( );
				}break;						
				case EV_NODE_CONFIG: {					
					BROWSE_FOR_NODES_TRACE("--> EV_NODE_CONFIG");
				} break;
				case EV_BROWSE_CLOSE_FILE: {				  
					this->helper_handleResponseCloseFile( );
				} break;
				case EV_NODE_NAME_READ_ERROR:
				case EV_NODE_NAME_READ_OK:
				case EV_NODE_NAME_END:
   				case EV_BROWSE_GET_UNIT_INFO: {	
					this->helper_handleWrongResponse( );
				}break;		
				case EV_BROWSE_USER_CANCEL: {				
					this->helper_handleUserCancel( );
				}break;
				case EV_BROWSE_FOR_NODES:
   				case EV_BROADCAST_TIMEOUT:
				case EV_BROWSE_NODEBINDING_INFO:
				case EV_NODE_RESOURCES0:
				case EV_NODE_RESOURCES1:
				case EV_NODE_RESOURCES2:
				case EV_NODE_RESOURCES3:
				case EV_NODE_RESOURCES4:
				default: {
					this->helper_displayWrongEventReceived( );		
				}break;
			 } // switch(iEvent)
		  } break; // STATE_GET_UNIT_NAME
	}// end switch currentstate
}

void CBrowseForNodesObject::setCurrentState(BrowseStates state) {
	this->m_CurrentState = state;
}
///////////////////////////////////////////////////////////////
void CBrowseForNodesObject::Start() {
	this->setCurrentState(STATE_BROWSE_IDLE);
	if (bNodeAddress == BROADCAST_NODE) {		
		pf_CurrentMode = &CBrowseForNodesObject::BrowseAllNodes;		
	}
	else {
		// TODO:
		// Eerst controleren of de node bestaat...
		pf_CurrentMode = &CBrowseForNodesObject::BrowseSingleNode;
	}
	this->BrowseForNodes( EV_BROWSE_FOR_NODES,-1,-1);
}

void CBrowseForNodesObject::Stop() {
	this->BrowseForNodes(EV_BROWSE_USER_CANCEL,-1,-1);
}

void CBrowseForNodesObject::BrowseForNodes(int iEvent, int wParam, int lParam) {
	ASSERT( NULL != pf_CurrentMode);
	if (NULL != pf_CurrentMode) {
		(this->*pf_CurrentMode)(iEvent,wParam,lParam);
	}
}


BOOL CBrowseForNodesObject::getBusyReadUnitName(BYTE bNodeAddress,BYTE bUnitAddress) {
	if (STATE_GET_UNIT_NAME != this->m_CurrentState) {
		return FALSE;
	}
	if (NULL == this->m_pNode) {
		return FALSE;
	}
	return ((this->m_pNode->getNodeAddress() == bNodeAddress) && (this->iUnitIndex == bUnitAddress));
}

BOOL CBrowseForNodesObject::getBusyReadNodeName(BYTE bNodeAddress) {
	if (STATE_GET_NODE_NAME != this->m_CurrentState) {
		return FALSE;
	}
	if (NULL == this->m_pNode) {
		return FALSE;
	}
	return (this->m_pNode->getNodeAddress() == bNodeAddress);
}


// Sedert V16.32: Timerwaarden verhoogd.
int CBrowseForNodesObject::getTimerBroadcastReloadValue( ) {	
	enum {
		TIMER_RELOAD_VALUE_DEFAULT 						= 20000,	// 20 sec ipv 18 sec	
		TIMER_RELOAD_VALUE_THRESSHOLD_1MODULE			= 7000,		// 7 sec ipv 5 sec
		TIMER_RELOAD_VALUE_THRESSHOLD_5MODULES			= 10000,	// 10 sec ipv 8 sec
		TIMER_RELOAD_VALUE_THRESSHOLD_50MODULES			= 30000,	// 30 sec ipv 25 sec
		TIMER_RELOAD_VALUE_THRESSHOLD_100MODULES		= 70000,	// 70 sec ipv 50 sec.
		TIMER_RELOAD_VALUE_THRESSHOLD_150MODULES	    = 90000,	// 90 sec ipv 70 sec
	};

	int foundNodes = 0;
	if (NULL != m_pDatabase) {
		int temp = 0;
		if (m_pDatabase->NROfNodes(&temp)) {
			foundNodes = temp;
		}
	}

	int returnValue = TIMER_RELOAD_VALUE_DEFAULT;
	if (foundNodes>=150) {
		returnValue = TIMER_RELOAD_VALUE_THRESSHOLD_150MODULES;
	}
	else if (foundNodes>=100) {
		returnValue = TIMER_RELOAD_VALUE_THRESSHOLD_100MODULES;
	}
	else if (foundNodes>=50) {
		returnValue = TIMER_RELOAD_VALUE_THRESSHOLD_50MODULES;
	}
	else if (foundNodes>=10) {
		returnValue = TIMER_RELOAD_VALUE_DEFAULT;
	}
	else if (foundNodes >= 5) {
		returnValue = TIMER_RELOAD_VALUE_THRESSHOLD_5MODULES;
	}
	else if (foundNodes >= 1) {
		returnValue = TIMER_RELOAD_VALUE_THRESSHOLD_1MODULE;
	}
	return returnValue;
}

bool CBrowseForNodesObject::checkNodeDatabaseValid( ) {	
	int nrOfNodes = 0;
	if (!m_pDatabase->NROfNodes(&nrOfNodes)) {
		ASSERT(0);
		return false;
	}				
	for (int i=0;i<nrOfNodes;++i) {	
		CCanNode* pNode = NULL;
		if (m_pDatabase->GetNode(i,&pNode)){
			ASSERT(NULL != pNode);
			if (CCanNode::NODE_TYPE_UNDEFINED == pNode->getNodeType()) {
				return false;
			}
		}
	}
	return true;
}

const CString CBrowseForNodesObject::getStrState(BrowseStates state) {
	switch(state) {
		case STATE_BROWSE_IDLE:
			return CString("STATE_BROWSE_IDLE");
		case STATE_BROWSE_NODES:
			return CString("STATE_BROWSE_NODES");
		case STATE_GET_NODE_NAME:
			return CString("STATE_GET_NODE_NAME");
		case STATE_GET_UNIT_NAME: 
			return CString("STATE_GET_UNIT_NAME");
		case STATE_GET_UNIT_INFO:
			return CString("STATE_GET_UNIT_INFO");
		case STATE_GET_NODE_RESOURCES:
			return CString("STATE_GET_NODE_RESOURCES");
		case STATE_GET_NODE_BINDINGFILEINFO:
			return CString("STATE_GET_NODE_BINDINGFILEINFO");
	}
	return CString("STATE_UNKNOWN");
}

void CBrowseForNodesObject::helper_handleUserCancel() {
	BROWSE_FOR_NODES_TRACE("--> EVENT : USER CANCEL");
	this->pBrowsePnpEngine->StopPnPAckTimer();
	this->setCurrentState(STATE_BROWSE_IDLE);
	this->pBrowsePnpEngine->Fire_OnBrowseCancel();
}

void CBrowseForNodesObject::helper_handleNoResponse() {
	BROWSE_FOR_NODES_TRACE("EV NORESPONSE");		
	this->pBrowsePnpEngine->StopPnPAckTimer();	
	CString szMessage;
	szMessage.Format("Error! No response in state %s.",getStrState(this->m_CurrentState));					
	CDisplayDevice::DisplayErrorMsg(szMessage);					
	this->pBrowsePnpEngine->Fire_OnBrowseTimeout();
}

void CBrowseForNodesObject::helper_handleResponseCloseFile() {	
	BROWSE_FOR_NODES_TRACE("--> EV_BROWSE_CLOSE_FILE");
	this->pBrowsePnpEngine->StopPnPAckTimer();																		
	CString szMessage(_T("ERROR, try to browse again..."));				
	CDisplayDevice::DisplayErrorMsg(szMessage);					
	this->pBrowsePnpEngine->Fire_OnBrowseError();
}

void CBrowseForNodesObject::helper_handleWrongResponse() {
	BROWSE_FOR_NODES_TRACE("--> EVENT : WRONG RESPONSE");	
	this->pBrowsePnpEngine->StopPnPAckTimer();													
	this->helper_displayWrongEventReceived( );		
	this->setCurrentState(STATE_BROWSE_IDLE);
	this->pBrowsePnpEngine->Fire_OnBrowseError();
}

void CBrowseForNodesObject::helper_displayWrongEventReceived() {	
	CString szMessage;
	szMessage.Format("Received wrong event in state %s.",getStrState(this->m_CurrentState));		
	CDisplayDevice::DisplayErrorMsg(szMessage);
}


