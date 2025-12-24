#include "stdafx.h"
#include "communicationconnectfsm.h"
#include "communication.h"
#include "serial/modemconnectfsm.h"
#include "tcpsocket/tcpsocketbase/tcpsocketconnectfsm.h"
#include "icomApplication.h"
#include "nodegenestructs.h"						// Applicatie - nodig voor het node type...

#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/*****************************************************************/
ICCommunicationConnectFsm::~ICCommunicationConnectFsm() { }
ISerialModemConnectFsm::~ISerialModemConnectFsm() { }
ICCommunicationConnectFsm_Host::~ICCommunicationConnectFsm_Host() { }
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationConnectFsm::CCommunicationConnectFsm(IComApplication *pCom) : 
	m_pImpl(NULL), m_pCommApplication(pCom), m_pCommunicationObject(NULL), m_serverInfo(), 
	m_allowConnectTouchscreen(false) {	 } 

CCommunicationConnectFsm::~CCommunicationConnectFsm() {
	if (NULL != m_pImpl) {
		delete m_pImpl;
		m_pImpl = NULL;
	}
}

// Bij de USB Modem FSM wordt er geen server info meegegeven. 
// De server info wordt upgedate in de CCommunicationObject klasse.
// Bij de tcp/ip implementatie geven we de server info mee + wordt de info ingevuld in deze fsm.
BOOL CCommunicationConnectFsm::startConnect(HWND hParent,CCommunicationObject *pCommObject) {
	if (NULL == m_pImpl) {
		if (NULL == pCommObject) {
			ASSERT(NULL != pCommObject);
			return FALSE;
		}		
		ICommunicationObject* const pCommObjectImpl = pCommObject->createImpl(hParent);
		if (NULL == pCommObjectImpl) {
			ASSERT(NULL != pCommObjectImpl);
			return FALSE;
		}
			
		m_pCommunicationObject = pCommObject;
		CCommunicationObject_Utils::clearServerInfo(pCommObject);

		// Server Info updaten...
		const CCommunicationObject::type_t type = m_pCommunicationObject->getType();
		if (CCommunicationObject::TCPIP == type) {			
			ICommunicationObject_Tcp* const pTemp = dynamic_cast<ICommunicationObject_Tcp*>(pCommObjectImpl);
			if (NULL != pTemp) {
				ITcpSocketConnectFsmFunctions* const pFsmFunctions = pTemp->getTcpSocketConnectFsmFunctions();
				m_pImpl = new CTCPSocketConnectFsm(*this,pFsmFunctions,m_serverInfo);
				ASSERT(NULL != m_pImpl);
			}
			else { ASSERT(0 != pTemp); }
		}
		else if (CCommunicationObject::SERIAL == type) {		
			m_pImpl = new CModemConnectFSM(*this,pCommObjectImpl);	
			ASSERT(NULL != m_pImpl);
		}
		else {
			ASSERT(CCommunicationObject::TCPIP == type || CCommunicationObject::SERIAL == type);
			return FALSE;
		}
	}
	if (NULL == m_pImpl) {
		ASSERT(NULL != m_pImpl);
		return FALSE;
	}
	m_pImpl->EvOperationConnect( );
	return TRUE;
}


// Seriele interface.
void CCommunicationConnectFsm::EvSignalAckReceived() {
	if (NULL == m_pImpl) {
		return;
	}
	ISerialModemConnectFsm* const pModemConnectFsm = dynamic_cast<ISerialModemConnectFsm*>(m_pImpl);
	if (NULL == pModemConnectFsm) {
		return;
	}	
	pModemConnectFsm->EvSignalAckReceived();		
}

void CCommunicationConnectFsm::EvAfterTimeout() {
	if (NULL == m_pImpl) {
		return;
	}
	m_pImpl->EvAfterTimeout();
}

void CCommunicationConnectFsm::EvSignalMessageReceived() {
	if (NULL == m_pImpl) {
		return;
	}
	ISerialModemConnectFsm* const pModemConnectFsm = dynamic_cast<ISerialModemConnectFsm*>(m_pImpl);
	if (NULL == pModemConnectFsm) {
		return;
	}	
	pModemConnectFsm->EvSignalMessageReceived();
}

void CCommunicationConnectFsm::EvSignalModemStatus() {
	if (NULL == m_pImpl) {
		return;
	}
	ISerialModemConnectFsm* const pModemConnectFsm = dynamic_cast<ISerialModemConnectFsm*>(m_pImpl);
	if (NULL == pModemConnectFsm) {
		return;
	}	
	pModemConnectFsm->EvSignalModemStatus();
}

void CCommunicationConnectFsm::EvSignalModemAddress() {
	if (NULL == m_pImpl) {
		return;
	}
	ISerialModemConnectFsm* const pModemConnectFsm = dynamic_cast<ISerialModemConnectFsm*>(m_pImpl);
	if (NULL == pModemConnectFsm) {
		return;
	}	
	pModemConnectFsm->EvSignalModemAddress();
}

void CCommunicationConnectFsm::EvSignalSoftwareVersion( ) {
	if (NULL == m_pImpl) {
		return;
	}
	ISerialModemConnectFsm* const pModemConnectFsm = dynamic_cast<ISerialModemConnectFsm*>(m_pImpl);
	if (NULL == pModemConnectFsm) {
		return;
	}	
	pModemConnectFsm->EvSignalSoftwareVersion();
}
/*****************************************************************/
void CCommunicationConnectFsm::displayMsgError(const CString& msg) {
	CDisplayDevice::DisplayErrorMsg(msg);
}
void CCommunicationConnectFsm::displayMsgInfo(const CString& msg) {
	CDisplayDevice::DisplayInfoMsg(msg);
}

void CCommunicationConnectFsm::startTimer(unsigned int timerID,unsigned int value){
	if (NULL == m_pCommApplication) {
		ASSERT(NULL != m_pCommApplication);
		return;
	}
	m_pCommApplication->StartTimer(timerID,value);
}	
void CCommunicationConnectFsm::stopTimer(unsigned int timerID) {
	if (NULL == m_pCommApplication) {
		ASSERT(NULL != m_pCommApplication);
		return;
	}
	m_pCommApplication->StopTimer(timerID);
}


// Updaten van de server info: Enkel bij de tcp/ip implementatie de ServerInfo updaten.
// Bij de modem wordt deze info niet upgedate in de fsm. (zie CCommunicationObject::onSerialReceivedModemCommand)
void CCommunicationConnectFsm::notifyFsmDone( ) {
	if (NULL == m_pCommApplication || NULL == m_pCommunicationObject) {
		ASSERT(NULL != m_pCommApplication);
		ASSERT(NULL != m_pCommunicationObject);
		return;
	}
	const CCommunicationObject::type_t type = m_pCommunicationObject->getType();
	if (CCommunicationObject::TCPIP == type) {		
		CCommunicationObject_Utils::updateServerInfo(m_pCommunicationObject,m_serverInfo);	
	}
	m_pCommApplication->NotifyOnTaskDone(COMAPPLICATION_TASK_ID_MODEMCONNECT_ENGINE);
	m_pCommApplication->NotifyConnectStatusChanged(TRUE);	// Sedert V16.07
}

void CCommunicationConnectFsm::notifyFsmError( ) {
	if (NULL == m_pCommApplication) {
		ASSERT(NULL != m_pCommApplication);
		return;
	}
	m_pCommApplication->NotifyOnTaskError(COMAPPLICATION_TASK_ID_MODEMCONNECT_ENGINE);	
}

// Configuratie via tcp/homeserver pro vanaf V66.08.xx
bool CCommunicationConnectFsm::checkFirmwareVersion(int softwareVersion) {
	bool result = false;
	const int major = ((softwareVersion & 0xff00) >> 8);
	const int minor = (softwareVersion & 0xFF);
	if (0x66 == major && minor >= 0x08) {
		result = true;
	}
	return result;
}


// Sedert V16.14.
// Normaal enkel via de tcp/ip homeserver maar wanneer 'enable_tcp' aanwezig is in de commandline bij het opstarten van de 
// applicatie dan ook configuratie toelaten via de touchscreen.
//
// TODO: Kunnen deze evt nog uitbreiden met een physical node address.
bool CCommunicationConnectFsm::checkNodeProperties(int nodeAddress, unsigned long ulPhysicalAddress, int nodeType, int nodeFlags) {	
	if (NODE_PIN_GATEWAY == nodeType) {
		return true;
	}
	if (NODE_LCD == nodeType && m_allowConnectTouchscreen) {
		return true;
	}
	return false;
}