#include "stdafx.h"
#include "communicationserverinfo.h"
/*****************************************************************/
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
CCommunicationServerInfo::CCommunicationServerInfo() : m_serialComport(0) { 
	clear();
}
CCommunicationServerInfo::~CCommunicationServerInfo() { }


void CCommunicationServerInfo::clear() {	
	m_type = OFFLINE;
	m_protocolVersion = 0;
	m_softwareVersion = 0;
	m_nodeAddress = 0xff;
	m_nodeName.Empty();
	m_ulPhysicalNodeAddress = 0xffffff;
	m_nodeType = NODE_UNKNOWN;
	// m_serialComport = -1; Niet wissen ??
	m_hostName.Empty();
	m_hostSocketPort = 0;
	m_password.Empty();
	m_connectionMethod = CONNECTION_METHOD_LOCAL;
	m_canLogSupported = false;
	m_serverMasterNode = false;
	m_serverMasterSupported = false;
}

void CCommunicationServerInfo::setCommunicationType(type_t type) {
	m_type = type;
}
CCommunicationServerInfo::type_t CCommunicationServerInfo::getCommunicationType() const {
	return m_type;
}

void CCommunicationServerInfo::setProtocolVersion(int version) {
	m_protocolVersion = version;
}
int CCommunicationServerInfo::getProtocolVersion() const {
	return m_protocolVersion;
}

void CCommunicationServerInfo::setSoftwareVersion(int version) {
	m_softwareVersion = version;
}
int CCommunicationServerInfo::getSoftwareVersion() const {
	return m_softwareVersion;
}

void CCommunicationServerInfo::setNodeAddress(int value) {
	m_nodeAddress = value;
}
int CCommunicationServerInfo::getNodeAddress() const {
	return m_nodeAddress;
}

void CCommunicationServerInfo::setPhysicalAddress(unsigned long value) {
	m_ulPhysicalNodeAddress = value;
}
unsigned long CCommunicationServerInfo::getPhysicalNodeAddress() const {
	return m_ulPhysicalNodeAddress;
}

void CCommunicationServerInfo::setNodeType(NodeType_t type) {
	m_nodeType = type;
}
CCommunicationServerInfo::NodeType_t CCommunicationServerInfo::getNodeType() const {
	return m_nodeType;
}

void CCommunicationServerInfo::setSerialComport(int value) {
	m_serialComport = value;
}
int CCommunicationServerInfo::getSerialComport() const {
	return m_serialComport;
}

void CCommunicationServerInfo::setNodeName(const CString& name) {
	m_nodeName = name;
}
const CString& CCommunicationServerInfo::getName() const {
	return m_nodeName;
}


void CCommunicationServerInfo::setTcpHostName(const CString& name) {
	m_hostName = name;
}
const CString& CCommunicationServerInfo::getTcpHostName( ) const {
	return m_hostName;
}

int CCommunicationServerInfo::getTcpSocketPort() const {
	return m_hostSocketPort;
}
void CCommunicationServerInfo::setTcpSocketPort(int v) {
	m_hostSocketPort = v;
}

void CCommunicationServerInfo::setPassword(const CString& pwd) {
	m_password = pwd;
}
const CString& CCommunicationServerInfo::getPassword( ) const {
	return m_password;
}

ConnectionMethod_t CCommunicationServerInfo::getConnectionMethod() const {
	return m_connectionMethod;
}
void CCommunicationServerInfo::setConnectionMethod(ConnectionMethod_t method) {
	m_connectionMethod = method;
}

// USB-Modem met CAN-Logger.
void CCommunicationServerInfo::setModemCanLogSupported(bool value) {
	m_canLogSupported = value;
}
BOOL CCommunicationServerInfo::getModemCanLogSupported( ) const {
	return m_canLogSupported;
}

// V16.26: tcp/ip server master node
void CCommunicationServerInfo::setServerMasterNode(bool stateMaster) {
	m_serverMasterNode = stateMaster;
}
bool CCommunicationServerInfo::getServerMasterNode() const {
	return m_serverMasterNode;
}

// V16.26: tcp/ip server master node
void CCommunicationServerInfo::setServerMasterNodeSupported(bool supported) {
	m_serverMasterSupported = supported;
}
bool CCommunicationServerInfo::getServerMasterNodeSupported() const {
	return m_serverMasterSupported;
}
/*****************************************************************/