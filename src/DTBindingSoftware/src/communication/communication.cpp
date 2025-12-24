#include "stdafx.h"
#include "BindingSoftware.h"
#include "communication.h"
#include "communicationstatus.h"
#include "serial/serialcommobject.h"			// Seriele implementatie.
#include "tcpsocket/tcpsocketcommobject.h"		// tcpip implementatie.

#include "IComApplication.h"
#include "nodegenestructs.h"					// Nodig voor de node vlaggen: NODE_MASTER_LCD
#include "tcpsocket/homeclient/tcpsockethomeclient.h"

#include "utils/app/DisplayDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
ICommunicationObject::~ICommunicationObject() { }
ICommunicationObject_Serial::~ICommunicationObject_Serial() { }
ICommunicationObject_Tcp::~ICommunicationObject_Tcp( ) { }

ICommunicationObject_Host::~ICommunicationObject_Host() { }
ISerialCommObject_Host::~ISerialCommObject_Host() { }
ITCPCommunicationObject_Host::~ITCPCommunicationObject_Host() { }

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationObject::CCommunicationObject(IComApplication& refParent) : 
	m_refParent(refParent), m_pImpl(0) , m_pCommunicationStatus(new CCommunicationStatus()), 
	m_settings(), m_pImplSerial(NULL), m_pImplTcp(NULL), m_mapHomeClients() // m_pTcpSocketHomeClient(NULL)
{
}

CCommunicationObject::~CCommunicationObject() 
{
	destroy();
}

CCommunicationObject::type_t CCommunicationObject::getType() const {
	if (0 == m_pImpl) {
		return NONE;
	}	
	{
		CSerialCommObject* const p = dynamic_cast<CSerialCommObject*>(m_pImpl); 
		if (0 != p) {
			return SERIAL;
		}	
	}
	{
		CTCPSocketCommObject* const p = dynamic_cast<CTCPSocketCommObject*>(m_pImpl); 
		if (0 != p) {
			return TCPIP;
		}
	}
	ASSERT(0);
	return NONE;
}

/*
bool CCommunicationObject::getTcpConnectedLocal( ) {
	const CCommunicationSettingsTcp* const pSettingsTcp = m_settings.getSettingsTcp();
	if (0 == pSettingsTcp) {
		ASSERT(0);
		return true;
	}
	return (CONNECTION_METHOD_LOCAL == pSettingsTcp->getConnectionMethod());
}
*/

ConnectionMethod_t CCommunicationObject::getTcpConnectionMethod() const {
	const CCommunicationSettingsTcp* const pSettingsTcp = m_settings.getSettingsTcp();
	if (0 == pSettingsTcp) {
		ASSERT(0);
		return CONNECTION_METHOD_LOCAL;
	}
	return pSettingsTcp->getConnectionMethod();
}

// In functie van de communicatatie instellingen een implementatie aanmaken...
ICommunicationObject* CCommunicationObject::createImpl(HWND hParent) {
	if (NULL == hParent) {
		return NULL;
	}
	const CCommunicationSettings::Method_t method = m_settings.getMethod();
	if (CCommunicationSettings::UNKNOWN == method) {
		ASSERT(method != CCommunicationSettings::UNKNOWN);
		return NULL;
	}
	if (CCommunicationSettings::TCPIP == method) {
		const CCommunicationSettingsTcp* const pSettingsTcp = m_settings.getSettingsTcp();
		if (NULL == pSettingsTcp || !pSettingsTcp->valid()) {
			ASSERT(NULL != pSettingsTcp);
			ASSERT(pSettingsTcp->valid());
			return NULL;
		}
		if (NULL == m_pImplTcp) {
			m_pImplTcp = new CTCPSocketCommObject(*this);
			ASSERT(NULL != m_pImplTcp);
			m_pImplTcp->Init(hParent);
		}
				
		ASSERT(NULL != m_pImplTcp);
		if (NULL != m_pImplTcp) {
			const CTCPSocketCommSettings tcpSettings(pSettingsTcp->getConnectionMethod(),pSettingsTcp->getHostName(),
													 pSettingsTcp->getPort(), pSettingsTcp->pwd(), pSettingsTcp->getCloudServerLoginToken());
			m_pImplTcp->setSettings(tcpSettings);

			// Pointer aanpassen !!
			m_pImpl = m_pImplTcp;			
		}
	}
	else if (CCommunicationSettings::USB == method) {
		const CCommunicationSettingsUsb* const pSettingsUsb = m_settings.getSettingsUsb();
		if (NULL == pSettingsUsb || !pSettingsUsb->valid()) {
			ASSERT(NULL != pSettingsUsb);
			ASSERT(pSettingsUsb->valid());
			return NULL;
		}

		if (NULL == m_pImplSerial) {
			m_pImplSerial = new CSerialCommObject(*this);	
			ASSERT(NULL != m_pImplSerial);
			m_pImplSerial->Init(hParent);	
		}

		ASSERT(NULL != m_pImplSerial);
		if (NULL != m_pImplSerial) {															
			// Settings doorgeven.
			m_pImplSerial->SetComPort(pSettingsUsb->getPort());

			// Pointer aanpassen !!
			m_pImpl = m_pImplSerial;	
		}
	}
	ASSERT(NULL != m_pImpl);
	return m_pImpl;
}

void CCommunicationObject::destroy() {
	m_pImpl = 0;

	if (0 != m_pImplSerial) {
		delete m_pImplSerial;
		m_pImplSerial = 0;
	}
	if (0 != m_pImplTcp) {
		delete m_pImplTcp;
		m_pImplTcp = 0;
	}
	if (0 != m_pCommunicationStatus) {
		delete m_pCommunicationStatus;
		m_pCommunicationStatus=0;
	}
	this->destroyHomeClients();
}

// Sedert V16.35: Bijhouden van de instanties van een homeclient in een map.
void CCommunicationObject::destroyHomeClients() {
	POSITION pos = this->m_mapHomeClients.GetStartPosition();
	int keyValue;	
	while (pos != NULL) {
		ITcpSocketHomeClient* pInstance = NULL;
		this->m_mapHomeClients.GetNextAssoc(pos, keyValue, pInstance);
		this->m_mapHomeClients.RemoveKey(keyValue);
		if (NULL != pInstance) {
			delete pInstance;
			pInstance = NULL;
		}
	}
	ASSERT(this->m_mapHomeClients.GetSize() == 0);
}

BOOL CCommunicationObject::IsConnectionOpen(void) {
	if (0 == m_pImpl) {
		return FALSE;	
	}
	return m_pImpl->IsConnectionOpen();
}
	
BYTE CCommunicationObject::MessageTransfer(BYTE bNodeAddress,BYTE bUnitAddress,BYTE bMsgCode,
								 BYTE bLength,const CString& szData)
{
	if (0 == m_pImpl) {
		return FALSE;	
	}
	return m_pImpl->MessageTransfer(bNodeAddress,bUnitAddress,bMsgCode,bLength,szData);
}

const ICommunicationFrame* CCommunicationObject::GetRxModemFrame() const {
	if (0 == m_pImpl) {
		return 0;	
	}
	return m_pImpl->GetRxModemFrame();
}


// Sedert V16.20: Onderscheid maken tussen de berichten die ontvangen worden...
CCommunicationObject::ResultRxMsg_t CCommunicationObject::GetRxMsg() {
	if (FALSE == GetRXMsgFromQueue()) {		
		return RESULT_RX_MSG_NONE;
	}	

	ResultRxMsg_t result = RESULT_RX_MSG_NONE;
	const ICommunicationFrame* const pCommunicationFrame = GetRxModemFrame();
	if (NULL != pCommunicationFrame) {
		const int modemCommand = pCommunicationFrame->GetModemCmd();
		if (MODEM_RXMESSAGE == modemCommand) {
			const CCommunicationAppFrame rxFrame(CCommunicationAppFrame::_MESSAGE_RECEIVED,pCommunicationFrame);
			getCommunicationStatus().setLastRxFrame(rxFrame);
			result = RESULT_RX_MSG_MODEMFRAME;
		}
		else if (MODEM_RXMESSAGE_CANLOG == modemCommand) {
			const CCommunicationAppFrame rxFrame(CCommunicationAppFrame::_MESSAGE_LOG,pCommunicationFrame);
			getCommunicationStatus().setLastRxFrame(rxFrame);		
			result = RESULT_RX_MSG_LOG;
		}
	}
	return result;
}

BOOL CCommunicationObject::GetRXMsgFromQueue(void) {
	if (0 == m_pImpl) {
		return FALSE;	
	}
	return m_pImpl->GetRXMsgFromQueue();
}

void CCommunicationObject::closeConnections( ) {
	if (0 != m_pImpl) {
		m_pImpl->CloseConnection();
	}
	this->destroyHomeClients();
}

void CCommunicationObject::CloseConnection() {
	if (0 == m_pImpl) {
		return;
	}
	m_pImpl->CloseConnection();

	//
	// TODO: Communication status wissen ??
	//

}

/************************* Opvragen van de status *********************/
CCommunicationStatus& CCommunicationObject::getCommunicationStatus() {
	ASSERT(NULL != m_pCommunicationStatus);
	return *m_pCommunicationStatus;
}
const CCommunicationStatus& CCommunicationObject::getCommunicationStatus() const {
	ASSERT(NULL != m_pCommunicationStatus);
	return *m_pCommunicationStatus;
}

/************************* Serial functions ***************************/
BYTE CCommunicationObject::SendSerialModemCommand(BYTE cmd) {
	if (0 == m_pImpl) {
		return FALSE;	
	}

	ICommunicationObject_Serial* const pSerialCommObject = dynamic_cast<ICommunicationObject_Serial*>(m_pImpl);
	if (NULL == pSerialCommObject) {
		return 0;
	}
	return pSerialCommObject->MessageTransfer(cmd);
}

void CCommunicationObject::EvAfterAckTimeout(void) {
	if (0 == m_pImpl) {
		return;
	}
	ICommunicationObject_Serial* const pSerialCommObject = dynamic_cast<ICommunicationObject_Serial*>(m_pImpl);
	if (NULL == pSerialCommObject) {
		return;
	}
	pSerialCommObject->EvAfterAckTimeout();
}

void CCommunicationObject::EvAfterRetryTimeout(void) {
	if (0 == m_pImpl) {
		return;
	}
	ICommunicationObject_Serial* const pSerialCommObject = dynamic_cast<ICommunicationObject_Serial*>(m_pImpl);
	if (NULL == pSerialCommObject) {
		return;
	}
	pSerialCommObject->EvAfterRetryTimeout();
}
	
// Tijdelijk...	
void CCommunicationObject::setSettings(const CCommunicationSettingsUsb& settings) {
	m_settings.clear();
	m_settings.set(settings);
}
void CCommunicationObject::setSettings(const CCommunicationSettingsTcp& settings) {
	m_settings.clear();
	m_settings.set(settings);
}

short CCommunicationObject::GetComPort(void) {
	if (0 == m_pImpl) {
		return 0;		// Is deze ook als return waarde ??
	}
	ICommunicationObject_Serial* const pSerialCommObject = dynamic_cast<ICommunicationObject_Serial*>(m_pImpl);
	if (NULL == pSerialCommObject) {
		return 0;
	}
	return pSerialCommObject->GetComPort( );
}

void CCommunicationObject::ResetSerialModem() {
	if (0 == m_pImpl) {
		return;	
	}
	ICommunicationObject_Serial* const pSerialCommObject = dynamic_cast<ICommunicationObject_Serial*>(m_pImpl);
	if (NULL == pSerialCommObject) {
		return;
	}
	pSerialCommObject->ResetModem();
}

BOOL CCommunicationObject::SendSerialModemCANLoggerOnOff(BOOL fState) {
	if (0 == m_pImpl) {
		return FALSE;
	}
	ICommunicationObject_Serial* const pSerialCommObject = dynamic_cast<ICommunicationObject_Serial*>(m_pImpl);
	if (NULL == pSerialCommObject) {
		return FALSE;
	}
	return pSerialCommObject->Send_CanLoggerEnable(fState);
}
	
void CCommunicationObject::ParseRxSerialData(void) {
	if (0 == m_pImpl) {
		return;	
	}
	ICommunicationObject_Serial* const pSerialCommObject = dynamic_cast<ICommunicationObject_Serial*>(m_pImpl);
	if (NULL == pSerialCommObject) {
		return;
	}
	pSerialCommObject->ParseRXMessage();
}

/*********************************************************************************************************/

// Sedert V16.36
// Instantie aanmaken mv de socketopties.
// Indien reeds aangemaakt de oude teruggeven....
ITcpSocketHomeClient* CCommunicationObject::createSocketHomeClient(int nodeAddress, const CTCPSocketOptions& refOptions)  {
	ITcpSocketHomeClient* pInstance = NULL;
	if (this->m_mapHomeClients.Lookup(nodeAddress,pInstance)) {		
		return pInstance;
	}
	pInstance = new CTCPSocketHomeClient(*this,refOptions);
	ASSERT(0 != pInstance);
	this->m_mapHomeClients.SetAt(nodeAddress,pInstance);	
	return pInstance;
}

// Sedert V16.36: In de map zoeken.
ITcpSocketHomeClient* CCommunicationObject::getSocketHomeClient(int nodeAddress) {
	ITcpSocketHomeClient* pInstance = NULL;
	if (this->m_mapHomeClients.Lookup(nodeAddress,pInstance)) {
		return pInstance;
	}	
	return NULL;
}

/*********************************************************************************************************/
// ICommunicationObject_Host
void CCommunicationObject::displayMsgInfo(int socketIdentifier, const CString& msg) {
	if ((NULL != m_pImplTcp) && (m_pImplTcp->GetIdentifier() == socketIdentifier)) {
		CDisplayDevice::DisplayInfoMsg(msg);
	}
}

void CCommunicationObject::displayMsgError(int socketIdentifier, const CString& msg) {
	if ((NULL != m_pImplTcp) && (m_pImplTcp->GetIdentifier() == socketIdentifier)) {
		CDisplayDevice::DisplayErrorMsg(msg);
	}
}


// ISerialCommObject_Host
void CCommunicationObject::onSerialReceivedMessage(void) {
		
	// TRACE("CCommunicationObject::onSerialReceivedMessage()\r\n");

	CSerialCanModem* const pSerialModem = getCommunicationStatus().GetModemInfo();
	if (NULL != pSerialModem) {
		// Restore application : Communication status !!!
		CSerialCanModemErrorStatus& refModemErrorStatus = pSerialModem->getErrorStatus();
		refModemErrorStatus.SetFatalErrorFlag(FALSE);
	}
	m_refParent.NotifyReceivedMessage();
}


void CCommunicationObject::onSerialReceivedModemCommand(void) {
	
	ASSERT(0 != m_pCommunicationStatus);

	if (GetRXMsgFromQueue()) 
	{			
		const ICommunicationFrame* const pModemFrame = GetRxModemFrame();
		if (NULL == pModemFrame) {	
			ASSERT(NULL != pModemFrame);
			return;
		}

		const CCommunicationAppFrame rxFrame(CCommunicationAppFrame::_COMMAND_RECEIVED,pModemFrame);
		getCommunicationStatus().setLastRxFrame(rxFrame);

		const BYTE bModemCommand = pModemFrame->GetModemCmd();
		switch(bModemCommand)	
		{
			case MODEM_STATUS:
			{				
				CSerialCanModem* const pSerialModem = m_pCommunicationStatus->GetModemInfo();
				if (NULL != pSerialModem) {
					pSerialModem->SetStatus(pModemFrame->GetCmdDataBYTE(0));				
					pSerialModem->SetV24Status(pModemFrame->GetCmdDataBYTE(2));												
				}	

				// Server info updaten met de software versie.
				CCommunicationServerInfo& refCommunicationServerInfo = m_pCommunicationStatus->getServerInfo();
				const int softwareversion = ((pModemFrame->GetCmdDataBYTE(3) << 8) + 0xFF);
				refCommunicationServerInfo.setSoftwareVersion(softwareversion);

				// Sedert V16.20
				if (0x90 == ((softwareversion & 0xFF00) >> 8)) {
					refCommunicationServerInfo.setModemCanLogSupported(true);
				}
				else {
					refCommunicationServerInfo.setModemCanLogSupported(false);
				}

				m_refParent.NotifyReceivedModemStatus();
				
			}break;			

			case MODEM_ADDRESS:
			{		
				// server info updaten met het node address.
				CCommunicationServerInfo& refCommunicationServerInfo = m_pCommunicationStatus->getServerInfo();
				refCommunicationServerInfo.setNodeAddress(pModemFrame->GetCmdDataBYTE(2));
			
				m_refParent.NotifyReceivedModemAddress();

			} break;

			case MODEM_SOFTWARE_VERSION:
			{				
				// Server info updaten met de software versie: MSB + LSB info.
				CCommunicationServerInfo& refCommunicationServerInfo = m_pCommunicationStatus->getServerInfo();
				const int softwareversion = ((pModemFrame->GetCmdDataBYTE(0) << 8) + pModemFrame->GetCmdDataBYTE(1));
				refCommunicationServerInfo.setSoftwareVersion(softwareversion);

				// Sedert V16.20
				if (0x90 == ((softwareversion & 0xFF00) >> 8)) {
					refCommunicationServerInfo.setModemCanLogSupported(true);
				}
				else {
					refCommunicationServerInfo.setModemCanLogSupported(false);
				}

				m_refParent.NotifyReceivedModemVersion();
				
			} break;

			case MODEM_CANLOG_STATUS:
			{
				BOOL fStatusOnOff = pModemFrame->GetCmdDataBYTE(0);
				m_pCommunicationStatus->setCanLoggerEnabled(fStatusOnOff);	

				m_refParent.NotifyModemCanLoggerStateChanged();

			} break;
		}
	} //if (m_pCommunicationObject->GetRXMsgFromQueue()) 
}


void CCommunicationObject::onSerialMessageSend(ICommunicationFrame* pFrame) {
	
	// TRACE("CCommunicationObject::onSerialMessageSend()\r\n");
	if (NULL == pFrame) {
		ASSERT(NULL != pFrame);
		return;
	}	
	if (PININSTALL_WRITE == pFrame->GetModemCmd()) {
		const CCommunicationAppFrame txFrame(CCommunicationAppFrame::_MESSAGE_SEND,pFrame);
		getCommunicationStatus().setLastTxFrame(txFrame);
	}
	else {
		const CCommunicationAppFrame txFrame(CCommunicationAppFrame::_COMMAND_SEND,pFrame);
		getCommunicationStatus().setLastTxFrame(txFrame);
	}	

	m_refParent.NotifyMessageSend();
}

void CCommunicationObject::onSerialReceivedAck(void) {

	// TRACE("CCommunicationObject::onSerialReceivedAck()\r\n");
	ASSERT(0 != m_pCommunicationStatus);

	const CCommunicationAppFrame rxFrame(CCommunicationAppFrame::_COMMAND_RECEIVED_ACK);
	getCommunicationStatus().setLastRxFrame(rxFrame);	

	// Restore application : Communication status !!!	
	CSerialCanModem* const pSerialModem = m_pCommunicationStatus->GetModemInfo();
	if (NULL != pSerialModem) {
		CSerialCanModemErrorStatus& refModemErrorStatus = pSerialModem->getErrorStatus();	
		refModemErrorStatus.SetFatalErrorFlag(FALSE);	
	}

	m_refParent.NotifyReceivedAck();
}

void CCommunicationObject::onSerialReceivedNack(void) {

	// TRACE("CCommunicationObject::onSerialReceivedNack()\r\n");
	ASSERT(0 != m_pCommunicationStatus);

	CSerialCanModem* const pSerialModem = m_pCommunicationStatus->GetModemInfo();
	if (NULL != pSerialModem) {
		CSerialCanModemErrorStatus& refModemErrorStatus = pSerialModem->getErrorStatus();
		refModemErrorStatus.SetFatalErrorFlag(FALSE);		
	}	

	const CCommunicationAppFrame rxFrame(CCommunicationAppFrame::_COMMAND_RECEIVED_NAK);
	getCommunicationStatus().setLastRxFrame(rxFrame);	

	// Doorgeven aan de parent.
	m_refParent.NotifyReceivedNack();
}


void CCommunicationObject::onSerialReceivedBadChecksum(void) {		
	CSerialCanModem* const pSerialModem = m_pCommunicationStatus->GetModemInfo();
	if (NULL != pSerialModem) {
		CSerialCanModemErrorStatus& refModemErrorStatus = pSerialModem->getErrorStatus();	
		refModemErrorStatus.SetType(CSerialCanModemErrorStatus::MODEM_ERROR_RECEIVED_BAD_CHECKSUM);
	}
	m_refParent.NotifyModemCommunicationError();
}

void CCommunicationObject::onSerialCommBuffersFull(void) {
	CSerialCanModem* const pSerialModem = m_pCommunicationStatus->GetModemInfo();
	if (NULL != pSerialModem) {
		CSerialCanModemErrorStatus& refModemErrorStatus = pSerialModem->getErrorStatus();	
		refModemErrorStatus.SetType(CSerialCanModemErrorStatus::MODEM_ERROR_BUFFERS_FULL);
	}
	m_refParent.NotifyModemCommunicationError();
}

void CCommunicationObject::onSerialNoResponse(void) {
	CSerialCanModem* const pSerialModem = m_pCommunicationStatus->GetModemInfo();
	if (NULL != pSerialModem) {
		CSerialCanModemErrorStatus& refModemErrorStatus = pSerialModem->getErrorStatus();		
		refModemErrorStatus.SetFatalErrorFlag(TRUE);
		refModemErrorStatus.SetType(CSerialCanModemErrorStatus::MODEM_ERROR_NO_RESPONSE);
	}
	m_refParent.NotifyModemCommunicationError();
}

void CCommunicationObject::onSerialMaxRetries(void) {
	CSerialCanModem* const pSerialModem = m_pCommunicationStatus->GetModemInfo();
	if (NULL != pSerialModem) {
		CSerialCanModemErrorStatus& refModemErrorStatus = pSerialModem->getErrorStatus();					
		refModemErrorStatus.SetFatalErrorFlag(TRUE);
		refModemErrorStatus.SetType(CSerialCanModemErrorStatus::MODEM_ERROR_MAX_ACK_RETRIES);				
	}
	m_refParent.NotifyModemCommunicationError();
}

void CCommunicationObject::startTimer(unsigned int id,unsigned int value) {
	m_refParent.StartTimer(id, value);
}
void CCommunicationObject::stopTimer(unsigned int id) {
	m_refParent.StopTimer(id);
}
// ISerialCommObject_Host - end

// ITCPCommunicationObject_Host
void CCommunicationObject::onTcpReceivedMessage(int socketIdentifier) {
	if ((NULL != m_pImplTcp) && (m_pImplTcp->GetIdentifier() == socketIdentifier)) {
		m_refParent.NotifyReceivedMessage();
	}
}

void CCommunicationObject::onTcpMessageSend(int socketIdentifier, ICommunicationFrame* pFrame) {
	if (NULL == pFrame) {
		ASSERT(NULL != pFrame);
		return;
	}
	
	if ((NULL != m_pImplTcp) && (m_pImplTcp->GetIdentifier() == socketIdentifier)) {
		const CCommunicationAppFrame txFrame(CCommunicationAppFrame::_MESSAGE_SEND,pFrame);
		getCommunicationStatus().setLastTxFrame(txFrame);		
		
		m_refParent.NotifyMessageSend();
	}
}

// Sedert V16.36: 
// Het event ook doorgeven aan de Homeclient observers.
// Vroeger werd dit event enkel naar de tcp/ip socket van de CAN-bus/Modem verstuurd.
void CCommunicationObject::onTcpSocketClosed(int socketIdentifier) {

	// TCP/IP Can Socket...
	if ((NULL != m_pImplTcp) && (m_pImplTcp->GetIdentifier() == socketIdentifier)) {
		m_refParent.NotifyConnectStatusChanged(FALSE);
	}

	// HomeClient sockets.
	POSITION pos = this->m_mapHomeClients.GetStartPosition();	
	while (pos != NULL) {
		int keyValue = -1;	
		ITcpSocketHomeClient* pInstance = NULL;
		this->m_mapHomeClients.GetNextAssoc(pos, keyValue, pInstance);	
		if (NULL != pInstance) {
			pInstance->notifyConnectStatusChanged( );
		}
	}
}

// Sedert V16.26: Nodig voor de DT18-PRO.
// Instellen van master of slave node address.
void CCommunicationObject::updateServerInfo(int nodeAddress, const char* name, int numberOfUnits, int nodeFlags)
{
	CCommunicationStatus& refCommStatus = this->getCommunicationStatus();

	if (refCommStatus.connected()) {
		CCommunicationServerInfo& refCommServerInfo = refCommStatus.getServerInfo();

		// Updaten van het node address.
		refCommServerInfo.setNodeAddress(nodeAddress);

		// Updaten van de naam.
		if (NULL != name) {
			const CString strName(name);
			refCommServerInfo.setNodeName(strName);
		}

		// Updaten van de status master/slave	
		refCommServerInfo.setServerMasterNode(NODE_MASTER_LCD == (nodeFlags & NODE_MASTER_LCD));	
	}
}

// ITCPCommunicationObject_Host - end

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

void CCommunicationObject_Utils::clearServerInfo(CCommunicationObject* pThis) {
	if (NULL == pThis) {
		ASSERT(NULL != pThis);
		return;
	}
	CCommunicationServerInfo& refServerInfo = pThis->getCommunicationStatus().getServerInfo();
	refServerInfo.clear();
}

void CCommunicationObject_Utils::updateServerInfo(CCommunicationObject* pThis, const CCommunicationServerInfo& serverInfo) {
	if (NULL == pThis) {
		ASSERT(NULL != pThis);
		return;
	}

	CCommunicationServerInfo& refServerInfo = pThis->getCommunicationStatus().getServerInfo();
	refServerInfo = serverInfo;

	// Updaten van de settings van de tcp...
	const CCommunicationSettingsTcp* pSettingsTcp = pThis->getSettings().getSettingsTcp();
	if (NULL != pSettingsTcp) {
		refServerInfo.setTcpHostName(pSettingsTcp->getHostName());
		refServerInfo.setPassword(pSettingsTcp->pwd());
		refServerInfo.setTcpSocketPort(pSettingsTcp->getPort());
	}
}

////////////////////////////////////////////////////////////////////////////
// Functies enkel mogelijk wanneer geconnecteerd zijn met een TCP/IP server.
////////////////////////////////////////////////////////////////////////////

// Sedert V16.26.
// Integratie van DT18-PRO.
// Opvragen van de node info van de server om te controleren of deze een geldig node address gekregen heeft.
bool CCommunicationObject_Utils::sendRequestServerNodeInfo(CCommunicationObject* pThis) {
	ITcpSocketConnectFsmFunctions* pFsmFunctions = NULL;
	if (!get(pThis,&pFsmFunctions)) {
		return false;
	}
	return (TRUE == pFsmFunctions->sendMsgRequestNodeInfo());
}

bool CCommunicationObject_Utils::sendCfgServerAsMaster(CCommunicationObject* pThis) {
	ITcpSocketConnectFsmFunctions* pFsmFunctions = NULL;
	if (!get(pThis,&pFsmFunctions)) {
		return false;
	}
	return (TRUE == pFsmFunctions->sendMsgCfgAsMaster());
}

// protected helper functie.
bool CCommunicationObject_Utils::get(CCommunicationObject* pThis,ITcpSocketConnectFsmFunctions** ppFunctions) {
	if (NULL == pThis) {
		ASSERT(NULL != pThis);
		return false;
	}
	if (CCommunicationObject::TCPIP != pThis->getType()) {
		return false;
	}
	if (NULL == pThis->m_pImplTcp) {
		ASSERT(NULL != pThis->m_pImplTcp);
		return false;
	}
	ITcpSocketConnectFsmFunctions* const pFsmFunctions =  pThis->m_pImplTcp->getTcpSocketConnectFsmFunctions();
	if (NULL == pFsmFunctions) {
		ASSERT(NULL != pFsmFunctions);
		return false;
	}		
	*ppFunctions = pFsmFunctions;
	return true;	
}