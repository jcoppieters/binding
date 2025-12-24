#ifndef INCLUDED_COMMUNICATION_SERVERINFO_H
#define INCLUDED_COMMUNICATION_SERVERINFO_H
/*****************************************************************/
#include "communication_def.h"
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
class CCommunicationServerInfo
{
public:
	CCommunicationServerInfo();
	virtual ~CCommunicationServerInfo();

	typedef enum {
		OFFLINE = 0, SERIAL , TCPIP
	} type_t;

	typedef enum {
		NODE_UNKNOWN = 0, NODE_MODEM, NODE_HOMESERVER, NODE_TOUCHSCREEN, 
	} NodeType_t;

	void clear();

	void setCommunicationType(type_t type);
	type_t getCommunicationType() const;

	void setProtocolVersion(int version);
	int getProtocolVersion() const;

	void setSoftwareVersion(int version);
	int getSoftwareVersion() const;

	void setNodeAddress(int value);
	int getNodeAddress() const;

	void setPhysicalAddress(unsigned long value);
	unsigned long getPhysicalNodeAddress() const;

	void setNodeType(NodeType_t type);
	NodeType_t getNodeType() const;

	void setSerialComport(int value);
	int getSerialComport() const;

	void setNodeName(const CString& name);
	const CString& getName() const;

	void setTcpHostName(const CString& name);
	const CString& getTcpHostName( ) const;

	int getTcpSocketPort() const;
	void setTcpSocketPort(int v);

	void setPassword(const CString& pwd);
	const CString& getPassword( ) const;

	ConnectionMethod_t getConnectionMethod( ) const;
	void setConnectionMethod(ConnectionMethod_t method);
	
	// USB-Modem met CAN-Logger.
	void setModemCanLogSupported(bool value);
	BOOL getModemCanLogSupported( ) const;

	// Sedert V16.26
	// Deze info wordt aangevuld in de FSM connect & runtime upgedate wanneer de nodevlaggen veranderen.
	// tcp/ip server ingesteld als master in het CAN-bus systeem.
	void setServerMasterNode(bool stateMaster);				
	bool getServerMasterNode() const;

	// Sedert V16.26
	// Deze info wordt aangevuld in de FSM connect.
	// Kan de server ingesteld worden als master in het CAN-bus systeem.
	void setServerMasterNodeSupported(bool supported);		
	bool getServerMasterNodeSupported() const;

private:	
	type_t m_type;						// type 
	int m_protocolVersion;
	int m_softwareVersion;
	int m_nodeAddress;
	unsigned long m_ulPhysicalNodeAddress;
	CString m_nodeName;
	NodeType_t m_nodeType;
	int m_serialComport;				// Seriele poort waarmee geconnecteerd werd.
	CString m_hostName;					// tcp
	int m_hostSocketPort;				// tcp 
	CString m_password;					// tcp	
	ConnectionMethod_t m_connectionMethod; // tcp verbinding (lokaal, remote, remote cloud)
	bool m_canLogSupported;				// USB-modem vanaf V90.xx
	bool m_serverMasterNode;			// ingesteld als master.
	bool m_serverMasterSupported;		// Kan de server ingesteld worden als master.
};
/*****************************************************************/
#endif 
/*****************************************************************/