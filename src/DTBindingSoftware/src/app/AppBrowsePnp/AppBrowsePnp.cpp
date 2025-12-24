// BrowsePnPEngine.cpp: implementation of the CBrowsePnPEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BindingSoftware.h"
#include "AppBrowsePnp.h"		
#include "AppBrowseForNodesFSM.h"	
#include "AppPlugAndPlayFSM.h"					
#include "ComApplication.h"	
// ---------------------------------------
#include "TMGlobals.h"
using namespace TMNodeDatabase;
// ---------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBrowsePnPEngine::CBrowsePnPEngine(IComApplication* pCom) :
pCommApplication(pCom)
{	
	this->m_currentBrowsePnpMode = IDLE_MODE;
	this->m_pBrowseObject = NULL;
	this->m_pPnpObject = NULL;
}

CBrowsePnPEngine::~CBrowsePnPEngine()
{
	if (m_pBrowseObject != NULL) {
		delete m_pBrowseObject;
		m_pBrowseObject = NULL;
	}
	if (m_pPnpObject != NULL) {
		delete m_pPnpObject;
		m_pPnpObject = NULL;
	}
}

////////////////////////////////////////////////////////////////////////

BOOL CBrowsePnPEngine::sendPnpPhysicalAck(unsigned long ulPhysicalAddress) {	
	if (NULL == pCommApplication != NULL) {		
		ASSERT(pCommApplication);	
		return FALSE;
	}		
	CString szData;
	szData.Format(_T("%06x"),ulPhysicalAddress);						
	return (0 == pCommApplication->MessageTransfer(BROADCAST_NODE, BROADCAST_UNIT, FC_PNPPHYSADRACK, 3,	szData) ? FALSE : TRUE);	
}

void CBrowsePnPEngine::StartPnPTimer(int reloadValue) { 
	ASSERT(pCommApplication);	
	if (pCommApplication != NULL) {		
		pCommApplication->StartTimer(TIMERID_BROWSE_TIMER, reloadValue);
	}
}

void CBrowsePnPEngine::StopPnPTimer(void) {
	ASSERT(pCommApplication);
	if (pCommApplication != NULL) {		
		pCommApplication->StopTimer(TIMERID_BROWSE_TIMER);
	}
}

void CBrowsePnPEngine::StartBrowseTimer(int reloadValue) {
	ASSERT(pCommApplication);
	if (pCommApplication != NULL) {	
		pCommApplication->StartTimer(TIMERID_BROWSE_TIMER,reloadValue);
	}
}

void CBrowsePnPEngine::StopBrowseTimer(void) {
	ASSERT(pCommApplication);	
	if (pCommApplication != NULL) {		
		pCommApplication->StopTimer(TIMERID_BROWSE_TIMER);
	}
}
////////////////////////////////////////////////////////////////////////
void CBrowsePnPEngine::StartPnPAckTimer(void) {
	ASSERT(pCommApplication);
	if (pCommApplication != NULL) {		
		pCommApplication->StartTimer(TIMERID_PNP_BROWSE_NORESPONSE,PNP_NORESPONSE_TIMEOUT);
	}
}

void CBrowsePnPEngine::StopPnPAckTimer(void) {
	ASSERT(pCommApplication);
	if (pCommApplication != NULL) {	
		pCommApplication->StopTimer(TIMERID_PNP_BROWSE_NORESPONSE);
	}
}

void CBrowsePnPEngine::StartPnpPollTimer(int reloadValue) {
	if (pCommApplication == NULL) {	
		ASSERT(NULL != pCommApplication);
		return;
	}
	pCommApplication->StartTimer(TIMERID_PNP_POLL,reloadValue);
}

void CBrowsePnPEngine::StopPnpPollTimer( ) {
	if (pCommApplication == NULL) {	
		ASSERT(NULL != pCommApplication);
		return;
	}
	pCommApplication->StopTimer(TIMERID_PNP_POLL);
}

///////////////////////////////////////////////////////////////////////
void CBrowsePnPEngine::BrowseEnded(int nResult) {
	switch(this->m_currentBrowsePnpMode) {
		case BROWSE_FOR_NODES:
		case BROWSE_FOR_PH_NODES:
		case BROWSE_SINGLE_NODE:
		{
			switch (nResult)
			{
				case BROWSE_OK: {
					pCommApplication->NotifyBrowseOk();
					if (NULL != m_pBrowseObject) {
						delete m_pBrowseObject;
						m_pBrowseObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case BROWSE_ERROR: {
					pCommApplication->NotifyBrowsePnpError();
					if (NULL != m_pBrowseObject) {
						delete m_pBrowseObject;
						m_pBrowseObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case BROWSE_USER_CANCEL: {
					pCommApplication->NotifyBrowsePnpCancel();
					if (NULL != m_pBrowseObject) {
						delete m_pBrowseObject;
						m_pBrowseObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case BROWSE_NORESPONSE: {
					pCommApplication->NotifyBrowsePnpTimeout();
					if (NULL != m_pBrowseObject) {
						delete m_pBrowseObject;
						m_pBrowseObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				default: {
					ASSERT(0);
				} break;
			}
		}break;	

		default:
			ASSERT(0);
		break;			
	}
}

void CBrowsePnPEngine::PnPEnded(int nResult)
{
	switch(this->m_currentBrowsePnpMode) {
		case AUTO_PNP: {

			this->StopPnpPollTimer();

			switch(nResult) {				
				case PNP_OK: {
					pCommApplication->NotifyPnpOk();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_ERROR:	{
					pCommApplication->NotifyBrowsePnpError();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}					
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_FACTORY_ADDRESS_FOUND:	{
					pCommApplication->NotifyPnpFactoryAddressFound();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_DUPLICATES_FOUND:	{
					pCommApplication->NotifyPnpDuplicatesFound();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_USER_CANCEL: {
					pCommApplication->NotifyBrowsePnpCancel();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_NORESPONSE: {
					pCommApplication->NotifyBrowsePnpTimeout();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;
			}
		}break;

		case BROWSE_FOR_PH_NODES: {

			this->StopPnpPollTimer();

			switch(nResult) {				
				case PNP_OK: {
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}						
					BrowseForNodes();
				} break;

				case PNP_ERROR:	{
					pCommApplication->NotifyBrowsePnpError();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}					
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_FACTORY_ADDRESS_FOUND: {
					pCommApplication->NotifyPnpFactoryAddressFound();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_DUPLICATES_FOUND: {
					pCommApplication->NotifyPnpDuplicatesFound();				
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case PNP_USER_CANCEL: {
					pCommApplication->NotifyBrowsePnpCancel();
					if (NULL != m_pPnpObject) {
						delete m_pPnpObject;
						m_pPnpObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;

				case BROWSE_NORESPONSE:
				{
					pCommApplication->NotifyBrowsePnpTimeout();
					if (NULL != m_pBrowseObject) {
						delete m_pBrowseObject;
						m_pBrowseObject = NULL;
					}
					this->m_currentBrowsePnpMode = IDLE_MODE;
				} break;
			}
		} break;

		default:
			ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////////////////
void CBrowsePnPEngine::FSMSendEvent(int iEvent,int wParam,int lParam)
{
	switch(this->m_currentBrowsePnpMode) {
		case BROWSE_FOR_NODES: 
		case BROWSE_SINGLE_NODE: {
			if (m_pBrowseObject != NULL) {
				m_pBrowseObject->BrowseForNodes(iEvent, wParam, lParam);
			}
		} break;
		case BROWSE_FOR_PH_NODES:	{
			if (m_pPnpObject != NULL) {
				ASSERT(m_pBrowseObject == NULL);
				m_pPnpObject->PnpStateMachine(iEvent,wParam,lParam);			
			}
			else {
				if (m_pBrowseObject != NULL) {
					m_pBrowseObject->BrowseForNodes(iEvent, wParam, lParam);
				}
			}					
		} break;
		case AUTO_PNP: {
			if (m_pPnpObject != NULL) {
				m_pPnpObject->PnpStateMachine(iEvent,wParam,lParam);
			}
		} break;
		case IDLE_MODE:	{	
			TRACE("\nERROR ---> RECEIVING EVENT WHEN IN IDLE MODE\n");
		} break;
	}
}

BOOL CBrowsePnPEngine::getBusyReadUnitName(BYTE bNodeAddress,BYTE bUnitAddress) {
	if (NULL == this->m_pBrowseObject) {
		return FALSE;
	}
	return this->m_pBrowseObject->getBusyReadUnitName(bNodeAddress,bUnitAddress);	
}

BOOL CBrowsePnPEngine::getBusyReadNodeName(BYTE bNodeAddress) {
	if (NULL == this->m_pBrowseObject) {
		return FALSE;
	}
	return this->m_pBrowseObject->getBusyReadNodeName(bNodeAddress);	
}
///////////////////////////////////////////////////////////////////
	
void CBrowsePnPEngine::BrowseForNodes(void)
{
	if ((m_pBrowseObject == NULL) && (m_pPnpObject == NULL)) 
	{			
		this->m_currentBrowsePnpMode = BROWSE_FOR_NODES;	
			
		if ((m_pBrowseObject = new CBrowseForNodesObject((IBrowsePnPEngine*) this,
														 TMGetRuntimeNodes()))
														 != 0)
		{
			m_pBrowseObject->Start();		
		}
	}
}

void CBrowsePnPEngine::BrowseForPhysicalNodes(void)
{
	if ((m_pBrowseObject == NULL) && (m_pPnpObject == NULL)) 
	{
		this->m_currentBrowsePnpMode = BROWSE_FOR_PH_NODES;	
		
		if ((m_pPnpObject = new CPnpEngine(PNP_MODE_RETRIEVE_INFO,
									       (IBrowsePnPEngine*) this,
									       TMGetRuntimeNodes())) 
										   != NULL)
		{
			m_pPnpObject->Start();	
		}	
	}
}

void CBrowsePnPEngine::BrowseSingleNode(BYTE bNodeAddress)
{
	if ((m_pBrowseObject == NULL) && (m_pPnpObject == NULL)) 
	{
		this->m_currentBrowsePnpMode = BROWSE_SINGLE_NODE;	

		if ((m_pBrowseObject = new CBrowseForNodesObject(bNodeAddress,
													    (IBrowsePnPEngine*) this,
													    TMGetRuntimeNodes()))
														!= 0)
		{
			m_pBrowseObject->Start();
		}	
	}
}

void CBrowsePnPEngine::AutoPnp(void)
{
	if ((m_pBrowseObject == NULL) && (m_pPnpObject == NULL)) 
	{
		this->m_currentBrowsePnpMode = AUTO_PNP;	
			
		if ((m_pPnpObject = new CPnpEngine(PNP_MODE_AUTOMATIC,
									      (IBrowsePnPEngine*) this,
										   TMGetRuntimeNodes())) 
										  != 0)
		{
			m_pPnpObject->Start();
		}
	}	
}

void CBrowsePnPEngine::UserCancel(void)  {
	FSMSendEvent(EV_USER_CANCEL);
	this->StopBrowseTimer();
	this->StopPnPAckTimer();
	this->StopPnpPollTimer();
}


